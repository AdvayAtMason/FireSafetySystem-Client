#Import all Necessary Modules
from flask import Flask, render_template
from flask_sqlalchemy import SQLAlchemy
from flask_login import UserMixin
from flask_wtf import wtforms

#Initialize the App and Database
app = Flask(__name__)
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///database.db"
db = SQLAlchemy(app)
app.config["SECRET_KEY"] = "thisisanotsosecretkey"

#User Class
class User(db.Model, UserMixin):
    id = db.Column(db.Integer, primary_key=True)
    username = db.Column(db.String(20), nullable=False, unique=True)
    password = db.Column(db.String(80), nullable=False)

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

@app.route("/login", methods=["GET", "POST"])
def login():
    return render_template("login.html")

@app.route("/register", methods=["GET", "POST"])
def register():
    return render_template("register.html")

#This is is the program to run the server on a Localhost PC.
#We recommend that you run this on a virtual machine (NOTE: All VENV Folders are ignored) on Windows 10 or newer
if __name__ == "__main__":
    app.run()
