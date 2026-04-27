from pathlib import Path
import subprocess
from flask import Flask, render_template, request, redirect, url_for, flash

app = Flask(__name__)
app.secret_key = "class-project-secret"

DASHBOARD_DIR = Path(__file__).resolve().parent
PROJECT_ROOT = DASHBOARD_DIR.parent
BLOCKLIST_FILE = PROJECT_ROOT / "config" / "blocklist.txt"

WINDOWS_SERVER = PROJECT_ROOT / "pi-hole.exe"
LINUX_SERVER = PROJECT_ROOT / "pi-hole"

server_process = None


def get_server_path():
    if WINDOWS_SERVER.exists():
        return WINDOWS_SERVER
    if LINUX_SERVER.exists():
        return LINUX_SERVER
    return None


def load_blocklist():
    if not BLOCKLIST_FILE.exists():
        return []

    with open(BLOCKLIST_FILE, "r", encoding="utf-8") as file:
        domains = [line.strip().lower() for line in file if line.strip()]

    return sorted(set(domains))


def save_blocklist(domains):
    with open(BLOCKLIST_FILE, "w", encoding="utf-8") as file:
        for domain in sorted(set(domains)):
            file.write(domain + "\n")


def is_server_running():
    global server_process

    if server_process is None:
        return False

    return server_process.poll() is None


def start_server():
    global server_process

    if is_server_running():
        print("Server already running.", flush=True)
        flash("Server is already running.")
        return

    server_path = get_server_path()

    if server_path is None:
        print("Server executable not found. Compile the C program first.", flush=True)
        flash("Server executable not found. Compile the C program first.")
        return

    server_process = subprocess.Popen(
        [str(server_path)],
        cwd=str(PROJECT_ROOT / "src")
    )

    print("DNS server started.", flush=True)
    flash("DNS server started.")


def stop_server():
    global server_process

    if not is_server_running():
        print("Server is not running.", flush=True)
        flash("Server is not running.")
        return

    server_process.terminate()
    server_process = None

    print("DNS server stopped.", flush=True)
    flash("DNS server stopped.")


@app.route("/")
def index():
    domains = load_blocklist()

    sort_order = request.args.get("sort", "asc")

    if sort_order == "desc":
        domains = sorted(domains, reverse=True)
    else:
        domains = sorted(domains)
        sort_order = "asc"

    print("Dashboard loaded.", flush=True)

    return render_template(
        "index.html",
        running=is_server_running(),
        blocked_domains=domains,
        blocked_count=len(domains),
        sort_order=sort_order
    )


@app.route("/turn-on", methods=["POST"])
def turn_on():
    start_server()
    return redirect(url_for("index"))


@app.route("/turn-off", methods=["POST"])
def turn_off():
    stop_server()
    return redirect(url_for("index"))


@app.route("/add", methods=["POST"])
def add_domain():
    domain = request.form.get("domain", "").strip().lower()
    domains = load_blocklist()

    print(f"Add clicked. Domain entered: {domain}", flush=True)

    if domain and domain not in domains:
        domains.append(domain)
        save_blocklist(domains)
        print(f"Added domain to blocklist.txt: {domain}", flush=True)
        flash(f"Added {domain}. Restart the server to activate.")
    else:
        print("Domain was empty or already exists.", flush=True)
        flash("Domain was empty or already exists.")

    return redirect(url_for("index"))


@app.route("/remove", methods=["POST"])
def remove_domain():
    domain = request.form.get("domain", "").strip().lower()
    domains = load_blocklist()

    print(f"Remove clicked. Domain selected: {domain}", flush=True)

    if domain in domains:
        domains.remove(domain)
        save_blocklist(domains)
        print(f"Removed domain from blocklist.txt: {domain}", flush=True)
        flash(f"Removed {domain}. Restart the server to activate.")
    else:
        print("Domain was not found.", flush=True)
        flash("Domain was not found.")

    return redirect(url_for("index"))


if __name__ == "__main__":
    print("Starting Flask GUI...", flush=True)
    app.run(debug=True)