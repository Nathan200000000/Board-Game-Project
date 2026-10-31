// Runs the compiled C++ game (game.wasm) with a tiny WASI implementation.
// stdin blocks with Atomics.wait until the page sends a line of input,
// which lets the unmodified std::cin code work in the browser.

const ESUCCESS = 0, EBADF = 8, EINVAL = 28, ENOENT = 44, ENOTDIR = 54;
const O_CREAT = 1, O_DIRECTORY = 2, O_EXCL = 4, O_TRUNC = 8, FDFLAG_APPEND = 1;
const ALL_RIGHTS = 0xFFFFFFFFFFFFFFFFn;
const TTY_RIGHTS = ALL_RIGHTS & ~((1n << 2n) | (1n << 5n)); // no seek/tell => isatty

class ProcExit { constructor(code) { this.code = code; } }

let ctrl, data, memory;
let files = {};          // path -> Uint8Array (in-memory filesystem)
let stdinBuf = new Uint8Array(0);
const fds = new Map();   // fd -> open file record
let nextFd = 4;
const enc = new TextEncoder(), dec = new TextDecoder();

const view = () => new DataView(memory.buffer);
const bytes = () => new Uint8Array(memory.buffer);

function readLine() {
  Atomics.store(ctrl, 0, 0);
  postMessage({ type: 'input' });
  Atomics.wait(ctrl, 0, 0);
  const len = Atomics.load(ctrl, 1);
  return data.slice(0, len);
}

function iovs(ptr, len) {
  const v = view(), out = [];
  for (let i = 0; i < len; i++) out.push([v.getUint32(ptr + i * 8, true), v.getUint32(ptr + i * 8 + 4, true)]);
  return out;
}

const wasi = {
  args_get: () => ESUCCESS,
  args_sizes_get: (c, s) => { view().setUint32(c, 0, true); view().setUint32(s, 0, true); return ESUCCESS; },
  environ_get: () => ESUCCESS,
  environ_sizes_get: (c, s) => { view().setUint32(c, 0, true); view().setUint32(s, 0, true); return ESUCCESS; },
  clock_time_get: (id, prec, out) => {
    view().setBigUint64(out, BigInt(Math.round((id === 0 ? Date.now() : performance.now()) * 1e6)), true);
    return ESUCCESS;
  },
  random_get: (ptr, len) => { crypto.getRandomValues(bytes().subarray(ptr, ptr + len)); return ESUCCESS; },
  proc_exit: (code) => { throw new ProcExit(code); },
  sched_yield: () => ESUCCESS,

  fd_prestat_get: (fd, buf) => {
    if (fd !== 3) return EBADF;
    view().setUint8(buf, 0); view().setUint32(buf + 4, 1, true);
    return ESUCCESS;
  },
  fd_prestat_dir_name: (fd, path, len) => {
    if (fd !== 3) return EBADF;
    bytes()[path] = 0x2f; // "/"
    return ESUCCESS;
  },
  fd_fdstat_get: (fd, buf) => {
    const v = view();
    let type, rights;
    if (fd <= 2) { type = 2; rights = TTY_RIGHTS; }
    else if (fd === 3) { type = 3; rights = ALL_RIGHTS; }
    else if (fds.has(fd)) { type = 4; rights = ALL_RIGHTS; }
    else return EBADF;
    v.setUint8(buf, type); v.setUint16(buf + 2, 0, true);
    v.setBigUint64(buf + 8, rights, true); v.setBigUint64(buf + 16, rights, true);
    return ESUCCESS;
  },
  fd_fdstat_set_flags: () => ESUCCESS,

  path_open: (dirfd, dirflags, pathPtr, pathLen, oflags, rb, ri, fdflags, outFd) => {
    if (dirfd !== 3) return EBADF;
    if (oflags & O_DIRECTORY) return ENOTDIR;
    const path = dec.decode(bytes().slice(pathPtr, pathPtr + pathLen)).replace(/^\/+/, '');
    const exists = path in files;
    if (!exists && !(oflags & O_CREAT)) return ENOENT;
    if (exists && (oflags & O_EXCL)) return EINVAL;
    if (!exists || (oflags & O_TRUNC)) files[path] = new Uint8Array(0);
    const fd = nextFd++;
    fds.set(fd, { path, pos: 0, append: !!(fdflags & FDFLAG_APPEND), dirty: !exists || !!(oflags & O_TRUNC) });
    view().setUint32(outFd, fd, true);
    return ESUCCESS;
  },
  fd_close: (fd) => {
    const f = fds.get(fd);
    if (!f) return fd <= 3 ? ESUCCESS : EBADF;
    if (f.dirty) postMessage({ type: 'file', path: f.path, data: files[f.path] });
    fds.delete(fd);
    return ESUCCESS;
  },
  fd_seek: (fd, offset, whence, out) => {
    const f = fds.get(fd);
    if (!f) return fd <= 2 ? 70 /* ESPIPE */ : EBADF;
    const size = files[f.path].length;
    const base = whence === 0 ? 0 : whence === 1 ? f.pos : size;
    f.pos = Math.max(0, base + Number(offset));
    view().setBigUint64(out, BigInt(f.pos), true);
    return ESUCCESS;
  },
  fd_write: (fd, iov, iovLen, outN) => {
    let n = 0;
    const chunks = iovs(iov, iovLen).map(([p, l]) => { n += l; return bytes().slice(p, p + l); });
    if (fd === 1 || fd === 2) {
      for (const c of chunks) postMessage({ type: 'out', data: c });
    } else {
      const f = fds.get(fd);
      if (!f) return EBADF;
      let cur = files[f.path];
      if (f.append) f.pos = cur.length;
      for (const c of chunks) {
        const end = f.pos + c.length;
        if (end > cur.length) { const g = new Uint8Array(end); g.set(cur); cur = g; }
        cur.set(c, f.pos); f.pos = end;
      }
      files[f.path] = cur; f.dirty = true;
    }
    view().setUint32(outN, n, true);
    return ESUCCESS;
  },
  fd_read: (fd, iov, iovLen, outN) => {
    const list = iovs(iov, iovLen);
    let n = 0;
    if (fd === 0) {
      if (stdinBuf.length === 0) stdinBuf = readLine();
      for (const [p, l] of list) {
        const take = Math.min(l, stdinBuf.length);
        bytes().set(stdinBuf.subarray(0, take), p);
        stdinBuf = stdinBuf.slice(take); n += take;
        if (stdinBuf.length === 0) break;
      }
    } else {
      const f = fds.get(fd);
      if (!f) return EBADF;
      const src = files[f.path];
      for (const [p, l] of list) {
        const take = Math.max(0, Math.min(l, src.length - f.pos));
        bytes().set(src.subarray(f.pos, f.pos + take), p);
        f.pos += take; n += take;
      }
    }
    view().setUint32(outN, n, true);
    return ESUCCESS;
  },
};

onmessage = async (e) => {
  const { sab, initialFiles } = e.data;
  ctrl = new Int32Array(sab, 0, 2);
  data = new Uint8Array(sab, 8);
  files = initialFiles || {};
  const mod = await WebAssembly.compileStreaming(fetch('game.wasm'));
  const imports = { wasi_snapshot_preview1: new Proxy(wasi, {
    get: (t, k) => t[k] || (() => { console.warn('WASI call not implemented:', k); return 52; /* ENOSYS */ }),
  }) };
  const inst = await WebAssembly.instantiate(mod, imports);
  memory = inst.exports.memory;
  let code = 0;
  try { inst.exports._start(); }
  catch (err) {
    if (err instanceof ProcExit) code = err.code;
    else { postMessage({ type: 'out', data: enc.encode('\n[crashed: ' + err + ']\n') }); code = 1; }
  }
  postMessage({ type: 'exit', code });
};
