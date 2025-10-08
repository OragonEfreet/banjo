#!/usr/bin/env python3
import argparse, os, re, sys

# Old name -> New name (use None to keep unchanged)
RENAME = {
  "bj_mat3_scale": "bj_mat3_mul_scalar",                      # disambiguate from geometric scaling
  "bj_mat3_mul_vec3": "bj_mat3_transform_vec3",               # describe effect on vector
  "bj_mat3_transform_point_2d": "bj_mat3_transform_point",    # type implies 2D; shorter
  "bj_mat3_set_scale": "bj_mat3_set_scaling_xy",              # clarify per-axis scaling
  "bj_mat3_set_shear": "bj_mat3_set_shear_xy",                # specify plane
  "bj_mat3_set_rotation": "bj_mat3_set_rotation_z",           # 2D == Z-rotation; be explicit

  "bj_mat3x2_set_scale": "bj_mat3x2_set_scaling_xy",         # per-axis scaling
  "bj_mat3x2_set_rotation": "bj_mat3x2_set_rotation_z",      # 2D rotation axis explicit
  "bj_mat3x2_transform_point_2d": "bj_mat3x2_transform_point",# consistent verb + affine
  "bj_mat3x2_transform_direction_2d": "bj_mat3x2_transform_dir",# consistent “dir” term

  "bj_mat4_scale": "bj_mat4_mul_scalar",                      # scalar multiply
  "bj_mat4_set_scale": "bj_mat4_scale_axes",                  # modifies A’s basis; not a builder
  "bj_mat4_rotate": "bj_mat4_rotate_axis_andle",                    # axis-angle explicit

  "bj_mat4x3_set_scale": "bj_mat4x3_set_scaling_xyz",        # per-axis scaling
  "bj_mat4x3_mul_vec3": "bj_mat4x3_transform_point",         # effect on point
  "bj_mat4x3_mul_direction_3d": "bj_mat4x3_transform_dir",   # effect on direction
}

def build_pattern(keys):
    # longest first to help the regex engine
    parts = sorted((re.escape(k) for k in keys), key=len, reverse=True)
    return re.compile(r"\b(?:%s)\b" % "|".join(parts))

def replace_text(text, rx):
    def repl(m):
        k = m.group(0)
        v = RENAME.get(k)
        return v if v is not None else k
    return rx.subn(repl, text)

def process_file(path, rx, dry_run=False):
    if not os.path.isfile(path):
        print(f"skip (not a file): {path}", file=sys.stderr)
        return 0
    try:
        data = open(path, "r", encoding="utf-8", errors="ignore").read()
    except Exception as e:
        print(f"skip (read error): {path} [{e}]", file=sys.stderr)
        return 0
    new, n = replace_text(data, rx)
    if n and not dry_run:
        try:
            open(path, "w", encoding="utf-8", errors="ignore").write(new)
        except Exception as e:
            print(f"error (write): {path} [{e}]", file=sys.stderr)
            return 0
    if n:
        print(("{path} : {n} replacements" if not dry_run else
               "[dry-run] {path} : {n} matches").format(path=path, n=n))
    return n

def main():
    ap = argparse.ArgumentParser(description="Whole-word API renamer over explicit file list.")
    ap.add_argument("files", nargs="+", help="Files to edit. Pass any list; no directories.")
    ap.add_argument("--dry-run", action="store_true", help="Report matches only.")
    args = ap.parse_args()

    if not RENAME:
        print("RENAME map is empty.", file=sys.stderr)
        return 1

    rx = build_pattern(RENAME.keys())
    total = 0
    for f in args.files:
        total += process_file(f, rx, dry_run=args.dry_run)
    print(f"Total replacements: {total}")
    return 0

if __name__ == "__main__":
    sys.exit(main())
