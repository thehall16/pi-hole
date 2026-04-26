from flask import Flask, render_template, request, redirect, url_for

app = Flask(__name__)

# TEMPORARY DEMO DATA
# Later, these should be replaced with real Pi-hole data.

# TO DO: Replace this with a real check to see if the Pi-hole server is running
pihole_running = True

# TO DO: Replace this with a real count from connected clients/devices
connected_device_count = 5

# TODO: Replace this list with data loaded from config/blocklist.txt
blocked_domains = [
    "ads.google.com",
    "doubleclick.net",
    "tracking.example.com",
    "annoying-ads.com"
]


@app.route("/")
def index():
    print("Dashboard page loaded", flush=True)

    # Sends current status and blocklist data to the HTML page
    return render_template(
        "index.html",
        running=pihole_running,
        blocked_domains=blocked_domains,
        blocked_count=len(blocked_domains),
        connected_device_count=connected_device_count
    )


@app.route("/turn-on", methods=["POST"])
def turn_on():
    global pihole_running

    # TO DO: Replace this with code that starts/enables the actual Pi-hole service
    if not pihole_running:
        pihole_running = True
        print("Turn On clicked. Pi-hole is now running.", flush=True)
    else:
        print("Turn On clicked, but Pi-hole was already running.", flush=True)

    return redirect(url_for("index"))


@app.route("/turn-off", methods=["POST"])
def turn_off():
    global pihole_running

    # TO DO: Replace this with code that stops/disables the actual Pi-hole service
    if pihole_running:
        pihole_running = False
        print("Turn Off clicked. Pi-hole is now stopped.", flush=True)
    else:
        print("Turn Off clicked, but Pi-hole was already stopped.", flush=True)

    return redirect(url_for("index"))


@app.route("/add", methods=["POST"])
def add_domain():
    domain = request.form.get("domain", "").strip().lower()

    print(f"Add clicked. Domain entered: {domain}", flush=True)

    # TO DO: Replace this with code that writes the new domain to config/blocklist.txt
    if domain and domain not in blocked_domains:
        blocked_domains.append(domain)
        print(f"Added domain: {domain}", flush=True)
    else:
        print("Domain was empty or already exists.", flush=True)

    return redirect(url_for("index"))


@app.route("/remove", methods=["POST"])
def remove_domain():
    domain = request.form.get("domain")

    print(f"Remove clicked. Domain selected: {domain}", flush=True)

    # TO DO: Replace this with code that removes the domain from config/blocklist.txt
    if domain in blocked_domains:
        blocked_domains.remove(domain)
        print(f"Removed domain: {domain}", flush=True)
    else:
        print("Domain was not found.", flush=True)

    return redirect(url_for("index"))


if __name__ == "__main__":
    print("Starting Flask GUI...", flush=True)

    # For laptop testing, this runs only on your computer.
    # Later on the Pi, change this to:
    # app.run(host="0.0.0.0", port=5000, debug=True)
    app.run(debug=True)