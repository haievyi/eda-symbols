"""
scripts/gen_docs.py

MkDocs pre-build hook that automatically synchronizes README.md, SYMBOLS.md,
CHANGELOG.md, and the generated Direct2D PNG symbol images into docs/.
"""

from pathlib import Path
import shutil
import subprocess

REPO_ROOT = Path(__file__).resolve().parent.parent
DOCS_DIR = REPO_ROOT / "docs"
ASSETS_DIR = DOCS_DIR / "assets" / "symbols"

SYMBOL_IMAGES = [
    "ieee_circuit_breaker.png",
    "ieee_disconnect_open.png",
    "ieee_disconnect_closed.png",
    "ieee_fuse.png",
    "ieee_transformer_2w.png",
    "ieee_earth_ground.png",
    "iec_circuit_breaker.png",
    "iec_disconnect_open.png",
    "iec_disconnect_closed.png",
    "iec_fuse.png",
    "iec_transformer_2w.png",
    "iec_earth_ground.png",
]

GALLERY_ROWS = [
    (
        "Circuit Breaker",
        "InLine (2-port)",
        "ieee_circuit_breaker.png",
        "iec_circuit_breaker.png",
    ),
    (
        "Disconnect Switch (Open)",
        "InLine (2-port)",
        "ieee_disconnect_open.png",
        "iec_disconnect_open.png",
    ),
    (
        "Disconnect Switch (Closed)",
        "InLine (2-port)",
        "ieee_disconnect_closed.png",
        "iec_disconnect_closed.png",
    ),
    (
        "Power Fuse",
        "InLine (2-port)",
        "ieee_fuse.png",
        "iec_fuse.png",
    ),
    (
        "2-Winding Transformer (2W)",
        "InLine (2-port)",
        "ieee_transformer_2w.png",
        "iec_transformer_2w.png",
    ),
    (
        "Earth Ground",
        "Shunt (1-port)",
        "ieee_earth_ground.png",
        "iec_earth_ground.png",
    ),
]


def _find_or_generate_images() -> None:
    """Locate exported PNGs in the workspace or run export_symbol_images if needed."""
    ASSETS_DIR.mkdir(parents=True, exist_ok=True)

    found_map: dict[str, Path] = {}
    for png_name in SYMBOL_IMAGES:
        for candidate in REPO_ROOT.rglob(png_name):
            if candidate.parent != ASSETS_DIR and "site" not in candidate.parts:
                found_map[png_name] = candidate
                break

    if len(found_map) < len(SYMBOL_IMAGES):
        for exe in REPO_ROOT.rglob("export_symbol_images*"):
            if exe.is_file() and exe.suffix.lower() in (".exe", ""):
                subprocess.run([str(exe)], cwd=str(ASSETS_DIR), check=False)
                break

    for png_name, src_path in found_map.items():
        dst_path = ASSETS_DIR / png_name
        if src_path.resolve() != dst_path.resolve():
            shutil.copy2(src_path, dst_path)


def _generate_index() -> None:
    """Pull README.md into docs/index.md and sanitize links for --strict mode."""
    readme_path = REPO_ROOT / "README.md"
    if not readme_path.exists():
        return

    content = readme_path.read_text(encoding="utf-8")
    # Rewrite relative LICENSE link to GitHub blob URL so mkdocs --strict passes
    content = content.replace(
        "](LICENSE)",
        "](https://github.com/haievyi/eda-symbols/blob/main/LICENSE)",
    )

    content += (
        "\n\n## Documentation Sections\n\n"
        "- **[Symbols & Visual Gallery](symbols.md)** — Supported components and rendered Direct2D previews.\n"
        "- **[Changelog](changelog.md)** — Release history and component additions.\n"
    )

    (DOCS_DIR / "index.md").write_text(content, encoding="utf-8")


def _generate_symbols_page() -> None:
    """Pull SYMBOLS.md into docs/symbols.md and append the rendered PNG gallery."""
    symbols_path = REPO_ROOT / "SYMBOLS.md"
    table_md = symbols_path.read_text(encoding="utf-8").strip() if symbols_path.exists() else ""

    lines = [
        "# Symbol Library & Visual Gallery",
        "",
        "## Supported Components",
        "",
        table_md,
        "",
        "## Rendered Symbol Gallery (Direct2D Output)",
        "",
        "All previews below are automatically rendered headlessly via Direct2D and WIC during the build:",
        "",
        "| Component | Topology | IEEE / ANSI Standard | IEC 60617 Standard |",
        "| --- | --- | :---: | :---: |",
    ]

    for title, topology, ieee_img, iec_img in GALLERY_ROWS:
        ieee_cell = (
            f"![{title} (IEEE)](assets/symbols/{ieee_img})"
            if (ASSETS_DIR / ieee_img).exists()
            else f"`{ieee_img}`"
        )
        iec_cell = (
            f"![{title} (IEC)](assets/symbols/{iec_img})"
            if (ASSETS_DIR / iec_img).exists()
            else f"`{iec_img}`"
        )
        lines.append(f"| **{title}** | {topology} | {ieee_cell} | {iec_cell} |")

    lines.append("")
    (DOCS_DIR / "symbols.md").write_text("\n".join(lines), encoding="utf-8")


def _generate_changelog_page() -> None:
    """Pull CHANGELOG.md into docs/changelog.md."""
    changelog_path = REPO_ROOT / "CHANGELOG.md"
    if not changelog_path.exists():
        return

    content = changelog_path.read_text(encoding="utf-8")
    (DOCS_DIR / "changelog.md").write_text(content, encoding="utf-8")


def on_pre_build(config, **kwargs) -> None:
    """MkDocs hook entry point executed before scanning docs_dir."""
    DOCS_DIR.mkdir(parents=True, exist_ok=True)
    _find_or_generate_images()
    _generate_index()
    _generate_symbols_page()
    _generate_changelog_page()
