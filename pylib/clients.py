import hashlib
from time import sleep, time
from Queue import LifoQueue

def gentoken(self, seed=False):
    if not seed:
        seed = self.id
    token=hashlib.sha224(seed).hexdigest()
    return token


class kinect_client(object):
    def __init__(self, id, data):
        self.id=id
        self.ip=data.get("ip", "127.0.0.1")
        self.tmp_token=data.get("tmp_token")
        self.token(gentoken())
        self.lastchange=time()
        self.ingressq=LifoQueue(maxsize=1)

class beamer_client(object):
    def __init__(self, id, data):
        self.id=id
        self.ip=data.get("ip", "127.0.0.1")
        self.tmp_token=data.get("tmp_token")
        self.token(gentoken())
        self.lastchange=time()
        self.egressq=LifoQueue(maxsize=1)

