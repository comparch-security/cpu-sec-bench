#!/usr/bin/env bash
#
# cpu-sec-bench platform "riscv64-nemu-linux": build the bootable guest image
# out/<runset>/fw_payload.bin from the staged corpus (run build-corpus.sh
# first).  The image is the classic NEMU layout assembled by workload-builder:
#
#   0x80000000 +0     gcpt (LibCheckpointAlpha startup, <= 1 MiB)
#   0x80100000 +1 MiB OpenSBI fw_jump
#   0x80200000 +2 MiB device tree (xiangshan, 128 MiB DRAM, 1 hart)
#   0x80400000 +4 MiB Linux Image
#   ...        MB-aligned  initramfs (rootfs.cpio, uncompressed newc)
#
# and out/<runset>/meta.json records the image/input provenance.
#
# Pinned inputs (cloned/downloaded once into out/src, then reused):
#   workload-builder      b7c321dec6ad6f4c78abcaefc722b59b4043503c
#   OpenSBI               cbf9f6734dd85a982c63e3cb5db7ffe09da839ca  (v1.9)
#   LibCheckpointAlpha    5169da768d18f4682a489eaabe7c071df6d6ef88
#   linux-6.18.56.tar.xz  sha256 4633bbda2ab95eaf2a7af4041236451d6759c72e6946265531af4b3aacefbf89
#   busybox-1.36.1.tar.bz2 sha256 b8cc24c9574d809e7279c3be349795c5d5ceb6fdf19ca709f80cde50e47de314
#
# Kernel 6.18.56, not the plan's original 6.1.83: the guest userspace is built
# by the apt cross toolchain whose glibc is RVA23U64 vector-using code, and
# kernel-side userspace vector support only exists from Linux 6.5
# (arch/riscv/kernel/vector.c, CONFIG_RISCV_ISA_V).  On 6.1.83 every guest
# process dies with SIGILL on its first vector instruction -- measured
# 2026-10-20: the static busybox dies in a glibc memset `vsetivli` at init, and
# a *dynamic* probe built with the same cross g++-13 dies even earlier, inside
# ld-linux itself (evidence: tmp/verification/.work/rvv-probe/boot.log).  The
# env repo's riscv64 lane pins this same kernel for this same reason
# (scripts/riscv64/build_kernel.sh asserts the two symbols below); they are
# pinned into the config here and asserted against the built .config.
#
# The two tarballs may be seeded from cpu-sec-test-env/build/downloads/ (they
# are already present there on this workstation) before any download.

set -euo pipefail

PLATFORM_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
source "$PLATFORM_DIR/env.sh"

BENCH_DIR="$PLATFORM_DIR/../.."
RUNSET="${RUNSET:-acc}"
DEST="$OUT/$RUNSET"
SRC="$OUT/src"
DL="$SRC/downloads"

WB_COMMIT=b7c321dec6ad6f4c78abcaefc722b59b4043503c
WB_URL=https://github.com/OpenXiangShan/workload-builder
OPENSBI_COMMIT=cbf9f6734dd85a982c63e3cb5db7ffe09da839ca
OPENSBI_URL=https://github.com/riscv-software-src/opensbi
LCA_COMMIT=5169da768d18f4682a489eaabe7c071df6d6ef88
LCA_URL=https://github.com/openxiangshan/LibCheckpointAlpha
KERNEL_VER=6.18.56
KERNEL_TARBALL="linux-$KERNEL_VER.tar.xz"
KERNEL_SHA256=4633bbda2ab95eaf2a7af4041236451d6759c72e6946265531af4b3aacefbf89
KERNEL_URL="https://cdn.kernel.org/pub/linux/kernel/v6.x/$KERNEL_TARBALL"
BUSYBOX_VER=1.36.1
BUSYBOX_TARBALL="busybox-$BUSYBOX_VER.tar.bz2"
BUSYBOX_SHA256=b8cc24c9574d809e7279c3be349795c5d5ceb6fdf19ca709f80cde50e47de314
BUSYBOX_URL="https://busybox.net/downloads/$BUSYBOX_TARBALL"
SEED_DIRS=(
    "/home/wsong83/proj/xiangshan/cpu-sec-test-env/build/downloads"
)

mkdir -p "$SRC" "$DL" "$OUT/generated-dts"

[ -x "$DEST/bench/run-test" ] || { echo "ERROR: staged corpus missing ($DEST/bench/run-test); run build-corpus.sh first" >&2; exit 1; }
# a half-staged DEST (scheduler present, lib/ empty, no registry) must not be
# consumed: manifest.json is written only after every staging step succeeded
[ -f "$DEST/manifest.json" ] || { echo "ERROR: $DEST/manifest.json missing; build-corpus.sh did not complete for RUNSET=$RUNSET" >&2; exit 1; }

ensure_checkout() { # url dir commit
    local url="$1" dir="$2" commit="$3" head
    if [ ! -d "$dir/.git" ]; then
        echo "   clone $url"
        git clone --quiet "$url" "$dir"
    fi
    if ! git -C "$dir" cat-file -e "$commit^{commit}" 2>/dev/null; then
        git -C "$dir" fetch --quiet origin "$commit"
    fi
    git -C "$dir" checkout --quiet --detach "$commit"
    head="$(git -C "$dir" rev-parse HEAD)"
    [ "$head" = "$commit" ] || { echo "ERROR: $dir is at $head, expected $commit" >&2; exit 1; }
}

fetch_tarball() { # tarball sha256 url
    local name="$1" sha="$2" url="$3" seed
    if [ -f "$DL/$name" ] && echo "$sha  $DL/$name" | sha256sum -c --quiet -; then
        echo "   cached $name"; return 0
    fi
    for seed in "${SEED_DIRS[@]}"; do
        if [ -f "$seed/$name" ] && echo "$sha  $seed/$name" | sha256sum -c --quiet -; then
            echo "   seed $name from $seed"; cp "$seed/$name" "$DL/$name"; return 0
        fi
    done
    echo "   download $url"
    curl -fL --retry 3 -o "$DL/$name.part" "$url"
    echo "$sha  $DL/$name.part" | sha256sum -c --quiet - || { echo "ERROR: sha256 mismatch for $name" >&2; rm -f "$DL/$name.part"; exit 1; }
    mv "$DL/$name.part" "$DL/$name"
}

is_riscv_elf() { # file -> 0 when ELF64 riscv
    python3 - "$1" <<'PY'
import sys
h = open(sys.argv[1], "rb").read(20)
ok = len(h) >= 20 and h[:4] == b"\x7fELF" and h[4] == 2 and (h[18] | (h[19] << 8)) == 243
sys.exit(0 if ok else 1)
PY
}

export SOURCE_DATE_EPOCH

echo "== [1/6] fetch pinned inputs into $SRC"
ensure_checkout "$WB_URL" "$SRC/workload-builder" "$WB_COMMIT"
mkdir -p "$SRC/bootloader"
ensure_checkout "$OPENSBI_URL" "$SRC/bootloader/opensbi" "$OPENSBI_COMMIT"
ensure_checkout "$LCA_URL" "$SRC/bootloader/LibCheckpointAlpha" "$LCA_COMMIT"
# build-sbi.sh reads its config and patch from the SBI source's parent dir
cp "$SRC/workload-builder/bootloader/opensbi.config" "$SRC/bootloader/opensbi.config"
cp "$SRC/workload-builder/bootloader/opensbi.patch" "$SRC/bootloader/opensbi.patch"
WB="$SRC/workload-builder"
fetch_tarball "$KERNEL_TARBALL" "$KERNEL_SHA256" "$KERNEL_URL"
fetch_tarball "$BUSYBOX_TARBALL" "$BUSYBOX_SHA256" "$BUSYBOX_URL"

echo "== [2/6] busybox (static)"
if [ ! -x "$OUT/busybox-$BUSYBOX_VER/_install/bin/busybox" ]; then
    [ -d "$OUT/busybox-$BUSYBOX_VER" ] || tar -C "$OUT" -xf "$DL/$BUSYBOX_TARBALL"
    make -C "$OUT/busybox-$BUSYBOX_VER" defconfig >/dev/null
    sed -i 's/^# CONFIG_STATIC is not set$/CONFIG_STATIC=y/' "$OUT/busybox-$BUSYBOX_VER/.config"
    # busybox 1.36.1's tc applet no longer compiles against modern kernel
    # headers (the CBQ constants are gone); the guest needs no traffic control
    sed -i 's/^CONFIG_TC=y$/# CONFIG_TC is not set/' "$OUT/busybox-$BUSYBOX_VER/.config"
    # CC= pins the versioned gcc-13: the unversioned riscv64-linux-gnu-gcc on
    # this workstation is a newer major (15.x), and the corpus is built with
    # g++-13; the env repo's riscv64 lane pins the same pair
    make -C "$OUT/busybox-$BUSYBOX_VER" -j"$JOBS" CROSS_COMPILE="$CROSS_COMPILE" CC="$CC" >/dev/null
    make -C "$OUT/busybox-$BUSYBOX_VER" install CROSS_COMPILE="$CROSS_COMPILE" CC="$CC" >/dev/null
fi
if "$READELF" -d "$OUT/busybox-$BUSYBOX_VER/_install/bin/busybox" | grep -q NEEDED; then
    echo "ERROR: busybox is not statically linked" >&2; exit 1
fi
is_riscv_elf "$OUT/busybox-$BUSYBOX_VER/_install/bin/busybox" || { echo "ERROR: busybox is not a riscv64 ELF" >&2; exit 1; }
[ -e "$OUT/busybox-$BUSYBOX_VER/_install/sbin/init" ] || { echo "ERROR: busybox has no init applet" >&2; exit 1; }

echo "== [3/6] Linux $KERNEL_VER Image"
mkdir -p "$OUT/kernel"
# the cache key carries the version, so a kernel bump can never silently reuse
# the previous kernel's Image; out/kernel/Image is the working copy stage [6/6]
# pads, the cached per-version file stays the pristine build product
if [ ! -f "$OUT/kernel/Image-$KERNEL_VER" ]; then
    [ -d "$OUT/linux-$KERNEL_VER" ] || tar -C "$OUT" -xf "$DL/$KERNEL_TARBALL"
    # wb's NEMU fragment minus CONFIG_INITRAMFS_SOURCE: the rootfs travels in
    # the DTB /chosen node instead (the firmware packer patches it in)
    grep -v '^CONFIG_INITRAMFS_SOURCE=' "$WB/br2-external/board/openxiangshan/nemu/linux.config" \
        > "$OUT/linux-$KERNEL_VER/.config"
    # userspace vector, the load-bearing pair (see the header): RISCV_ISA_V is
    # the kernel-side vector support, RISCV_ISA_V_DEFAULT_ENABLE makes
    # riscv_v_default_allow default to allow -- glibc never issues the V prctl
    printf 'CONFIG_RISCV_ISA_V=y\nCONFIG_RISCV_ISA_V_DEFAULT_ENABLE=y\n' \
        >> "$OUT/linux-$KERNEL_VER/.config"
    # CC= pins the same versioned gcc-13 as the corpus (see the busybox note)
    make -C "$OUT/linux-$KERNEL_VER" ARCH=riscv CROSS_COMPILE="$CROSS_COMPILE" CC="$CC" olddefconfig >/dev/null
    make -C "$OUT/linux-$KERNEL_VER" ARCH=riscv CROSS_COMPILE="$CROSS_COMPILE" CC="$CC" -j"$JOBS" WERROR=0 Image
    for sym in CONFIG_RISCV_ISA_V CONFIG_RISCV_ISA_V_DEFAULT_ENABLE; do
        grep -qx "$sym=y" "$OUT/linux-$KERNEL_VER/.config" \
            || { echo "ERROR: built .config lacks $sym=y; guest userspace vector would SIGILL" >&2; exit 1; }
    done
    cp "$OUT/linux-$KERNEL_VER/arch/riscv/boot/Image" "$OUT/kernel/Image-$KERNEL_VER"
fi
cp "$OUT/kernel/Image-$KERNEL_VER" "$OUT/kernel/Image"

echo "== [4/6] gcpt and OpenSBI (DEFAULT_DTB=xiangshan)"
if [ ! -f "$OUT/gcpt/build/gcpt.bin" ]; then
    CROSS_COMPILE="$CROSS_COMPILE" DEFAULT_DTB=xiangshan DTS_TEMPLATE_DIR="$OUT/generated-dts" \
        bash "$WB/scripts/build-gcpt.sh" "$SRC/bootloader/LibCheckpointAlpha" "$OUT/gcpt"
fi
if [ ! -f "$OUT/opensbi/build/platform/generic/firmware/fw_jump.bin" ]; then
    CROSS_COMPILE="$CROSS_COMPILE" DEFAULT_DTB=xiangshan DTS_TEMPLATE_DIR="$OUT/generated-dts" \
        bash "$WB/scripts/build-sbi.sh" "$SRC/bootloader/opensbi" "$OUT/opensbi"
fi
# gcpt.bin is the raw payload the image carries; the ELF it was objcopy'd
# from is the sibling `gcpt`, and that one must be riscv64 (a host-compiled
# gcpt would boot nothing)
is_riscv_elf "$OUT/gcpt/build/gcpt" || { echo "ERROR: gcpt (the ELF) is not a riscv64 ELF" >&2; exit 1; }
[ -s "$OUT/gcpt/build/gcpt.bin" ] || { echo "ERROR: gcpt.bin is empty" >&2; exit 1; }

echo "== [5/6] rootfs tree and cpio"
rm -rf "$OUT/rootfs"
mkdir -p "$OUT/rootfs"/{bench,lib,etc,proc,sys,tmp,dev}
cp -a "$OUT/busybox-$BUSYBOX_VER/_install/." "$OUT/rootfs/"
ln -sf bin/busybox "$OUT/rootfs/init"
install -m 0644 "$PLATFORM_DIR/guest/inittab" "$OUT/rootfs/etc/inittab"
cp -a "$DEST/bench/." "$OUT/rootfs/bench/"
cp -a "$DEST/lib/." "$OUT/rootfs/lib/"
"$CC" -O2 -static -o "$OUT/rootfs/bench/nemu-trap" "$PLATFORM_DIR/guest/nemu-trap.c"
install -m 0755 "$PLATFORM_DIR/guest/bench.sh" "$OUT/rootfs/bench/bench.sh"
is_riscv_elf "$OUT/rootfs/bench/nemu-trap" || { echo "ERROR: nemu-trap is not a riscv64 ELF" >&2; exit 1; }

# Byte reproducibility: the tree above was just assembled by mkdir/cp/install,
# whose wall-clock mtimes would otherwise be archived by cpio (the kernel and
# busybox builds already honour SOURCE_DATE_EPOCH).  Stamp every entry -- -h so
# symlinks are stamped themselves, not their targets -- and feed cpio a sorted
# file list so the archive does not depend on readdir order either.
find "$OUT/rootfs" -exec touch -h -d "@$SOURCE_DATE_EPOCH" {} +
( cd "$OUT/rootfs" && find . | LC_ALL=C sort | cpio -o -H newc --owner=0:0 ) > "$DEST/rootfs.cpio" 2>/dev/null
echo "   rootfs.cpio: $(stat -c%s "$DEST/rootfs.cpio") bytes"

echo "== [6/6] assemble fw_payload.bin"
# The kernel's runtime footprint is larger than its Image file -- BSS lives
# past the last PROGBITS section -- so workload-builder's assembler, which
# places the initramfs at the first MiB boundary after the *file*, can land it
# inside the kernel's own reserved range; the kernel then discards it
# ("INITRD: ... overlaps in-use memory region") and panics with no root fs.
# Pad the Image until that boundary clears _end by a full MiB; the padded
# tail is zeros beyond the kernel that nothing ever reads (the kernel zeroes
# its own BSS).
source "$WB/scripts/dts-config.sh"
read -r MEM_BEGIN MEM_SIZE CLINT_MMIO <<< "$(dts_extract_config "$OUT/generated-dts/xiangshan.dts.in")"
# both spellings normalised here: dts_linux_kernel_address prints a DECIMAL
# address while System.map addresses are bare hex (an earlier version read the
# decimal through int(_, 16) as well, which garbled only the report line below
# -- the padding arithmetic never used that value)
KERNEL_BEGIN=$(( $(dts_linux_kernel_address "$MEM_BEGIN" $((4 * 1024 * 1024))) ))
KERNEL_START_V=$(( 0x$(awk '$3=="_start"{print $1; exit}' "$OUT/linux-$KERNEL_VER/System.map") ))
KERNEL_END_V=$(( 0x$(awk '$3=="_end"{print $1; exit}' "$OUT/linux-$KERNEL_VER/System.map") ))
python3 - "$OUT/kernel/Image" "$KERNEL_BEGIN" "$KERNEL_START_V" "$KERNEL_END_V" <<'PY'
import os, sys
image = sys.argv[1]
begin, start_v, end_v = (int(a) for a in sys.argv[2:5])
footprint = end_v - start_v      # link-time virtual span == the physical span here
mib = 1024 * 1024
need = (footprint + mib + mib - 1) // mib * mib
size = os.path.getsize(image)
if size < need:
    with open(image, "ab") as f:
        f.write(b"\0" * (need - size))
    print("   kernel Image padded %d -> %d bytes (clears _end by 1 MiB;" % (size, need))
    print("   initramfs will land at 0x%x, kernel _end at 0x%x)" % (begin + need, begin + footprint))
PY

CROSS_COMPILE="$CROSS_COMPILE" DTC=dtc DEFAULT_DTB=xiangshan \
    FIRMWARE_OUTPUT="$DEST/fw_payload.bin" \
    bash "$WB/scripts/build-firmware-linux.sh" \
        "$OUT/gcpt/build/gcpt.bin" "$OUT/opensbi" "$OUT/generated-dts" "$OUT/kernel/Image" "$DEST"

python3 - "$DEST" "$NEMU_BIN" "$RUNSET" "$BENCH_DIR" "$CXX" <<'PY'
import hashlib, json, os, subprocess, sys
dest, nemu, runset, bench, cxx = sys.argv[1:6]
def sha(p):
    h = hashlib.sha256()
    with open(p, "rb") as f:
        for c in iter(lambda: f.read(1 << 20), b""): h.update(c)
    return h.hexdigest()
meta = {
    "plan": "2026-10-11-riscv64-nemu-linux-platform",
    "platform": "riscv64-nemu-linux",
    "runset": runset,
    "bench_commit": subprocess.run(["git", "-C", bench, "rev-parse", "HEAD"],
                                   capture_output=True, text=True).stdout.strip(),
    # `-dirty` means the image was built from an uncommitted bench tree: the
    # commit alone would not reproduce the scheduler baked into it.
    "bench_tree": subprocess.run(["git", "-C", bench, "describe", "--always", "--dirty"],
                                 capture_output=True, text=True).stdout.strip(),
    "toolchain": subprocess.run([cxx, "--version"], capture_output=True, text=True).stdout.splitlines()[0].strip(),
    "fw_payload_bin": {"path": os.path.join(dest, "fw_payload.bin"), "sha256": sha(os.path.join(dest, "fw_payload.bin"))},
    "rootfs_cpio": {"path": os.path.join(dest, "rootfs.cpio"), "sha256": sha(os.path.join(dest, "rootfs.cpio"))},
    "nemu_bin": {"path": nemu, "sha256": sha(nemu) if os.path.exists(nemu) else None},
    "pins": {
        "workload_builder": "b7c321dec6ad6f4c78abcaefc722b59b4043503c",
        "opensbi": "cbf9f6734dd85a982c63e3cb5db7ffe09da839ca",
        "libcheckpointalpha": "5169da768d18f4682a489eaabe7c071df6d6ef88",
        "linux": "6.18.56 (4633bbda2ab95eaf2a7af4041236451d6759c72e6946265531af4b3aacefbf89)",
        "busybox": "1.36.1 (b8cc24c9574d809e7279c3be349795c5d5ceb6fdf19ca709f80cde50e47de314)",
    },
}
with open(os.path.join(dest, "meta.json"), "w") as f:
    json.dump(meta, f, indent=2, sort_keys=True); f.write("\n")
print("   fw_payload.bin: %d bytes, sha256 %s" % (os.path.getsize(os.path.join(dest, "fw_payload.bin")), meta["fw_payload_bin"]["sha256"][:16]))
PY

echo "== done: $DEST/fw_payload.bin"