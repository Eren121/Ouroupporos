from .ouroupporos import Ouroupporos
import glob

def main():
    ouroupporos = Ouroupporos(100)
    ouroupporos.generate_headers()