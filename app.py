from flask import Flask, render_template

#Initialize the App
app = Flask(__name__)

#This is how we will route people to the webpage. To route to a new webpage, simply make a new version of this.
#We can also use these functions to make custom methods. Look up the flask documentation for how to pass variables in.
@app.route("/", methods=["GET"])
def index():
    return render_template("index.html")

@app.route("/about", methods=["GET"])
def about():
    return render_template("about.html")

@app.route("/contact", methods=["GET"])
def contact():
    return render_template("contact.html")

#This is is the program to run the server on a Localhost PC.
#We recommend that you run this on a virtual machine (NOTE: All VENV Folders are ignored) on Windows 10 or newer
if __name__ == "__main__":
    app.run()
