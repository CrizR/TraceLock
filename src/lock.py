
import random
import pymongo
import logging
import datetime


class Lock(object):

    def __init__(self, lock_id=random.randint(100000000000, 999999999999), gps_location=[], email=None, phone=None,
                 init_time=None, broken_time=None, broken=False):
        self.lock_id = lock_id
        self.gps_location = gps_location
        self.email = email
        self.phone = phone
        self.init_time = init_time
        self.broken_time = broken_time
        self.broken = broken
        # Will change host later
        self.db = pymongo.MongoClient('localhost')["TraceLockDB"]
        self.locks = self.db["UserLocks"]
        self.initialize_lock()

    def initialize_lock(self):
        logging.info("Initializing Lock")
        lock = self.locks.find_one({"lock_id": self.lock_id})
        if lock is None:
            self.locks.insert_one({
                "lock_id": self.lock_id,
                # Would you need a user id?
                # "user_id": tbg
                "broken": False,
                "email": self.email,
                "phone": self.phone,
                "location_data": self.gps_location,
                "initialized_at": datetime.datetime.now(),
                "broken_at": None,
            })

    def update_lock(self, gps, email, phone):
        lock = self.locks.find_one({"lock_id": self.lock_id})
        if lock is None:
            logging.error("Lock not found")
        else:
            locations = lock["location_data"].append({"coordinates": gps, "time": datetime.datetime.now()})
            self.locks.update_one({"lock_id": self.lock_id},
                                  {"$set": {"locations": locations, "email": email, "phone": phone}})

    def break_lock(self, gps):
        lock = self.locks.find_one({"lock_id": self.lock_id})
        if lock is None:
            logging.error("Lock not found")
        else:
            locations = lock["location_data"].append({"coordinates": gps, "time": datetime.datetime.now()})
            self.locks.update_one({"lock_id": self.lock_id},
                                  {"$set": {"location_data": locations, "broken": True, "broken_at": datetime.datetime.now()}})

    def get_broken_path(self):
        lock = self.locks.find_one({"lock_id": self.lock_id})
        coord_path = []
        if lock is None:
            logging.error("Lock not found")
        else:
            for data_point in lock["location_data"]:
                if data_point["time"] >= lock["broken_at"]:
                    coord_path.append(data_point["coordinates"])
        return coord_path


class LockUtils(object):

    @staticmethod
    def get_lock(lock_id):
        db = pymongo.MongoClient('localhost')["TraceLockDB"]
        locks = db["UserLocks"]
        lock = locks.find_one({"lock_id": lock_id})
        if lock:
            logging.info("Returning lock " + lock["lock_id"])
            return Lock(lock["lock_id"], lock["location_data"], lock["email"], lock["phone"], lock["init_time"],
                        lock["broken_time"], lock["broken"])
        else:
            logging.error("Lock not found")










