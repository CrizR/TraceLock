from flask import Flask, jsonify, make_response, render_template
app = Flask(__name__)
from src.lock import *


@app.route("/", methods=['POST', 'GET'])
def intro():
    """
    With no path specified, returns the following welcome message.
    :return: a JSON Welcome
    """
    return render_template('index.html')


@app.route("/initialize", methods=['POST'])
def init_lock():
    lock = Lock()
    lock_data = {
        "lock_id": lock.lock_id,
    }
    return make_response(lock_data, 200)


@app.route("/<string:lock_id>/<string:gps>", methods=['POST'])
def update_lock(lock_id, gps):
    lock = LockUtils.get_lock(lock_id)
    lock.update_lock(gps, lock.email, lock.phone)


"""
For notifying
from twilio.rest import Client
message = self.client.messages.create(to=phone_number, from_=HOST_NUMBER, body=msg)
"""


if __name__ == '__main__':
    app.run(debug=True)