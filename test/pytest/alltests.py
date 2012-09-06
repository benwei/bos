import unittest,os
from ctypes import *

app_path = os.path.dirname(os.path.abspath(__file__))
libpathname = os.path.join(app_path, "./libbosc.so")
bc = CDLL(libpathname);

verbose = 0

def boslog(msg):
    global verbose
    if verbose:
        print msg


class BOSTest_atoi(unittest.TestCase):
    s = (c_byte*9)()
    c = 427

    def test_atoi(self):
        '''
        const char *itohex(uint32_t c, char *s, int size, int upper)
        '''
        b = bc.itohex(self.c, self.s, c_int(9), c_int(0))
        hex_str = string_at(self.s)
        boslog("%s" % (hex_str))
        assert hex_str == "000001ab", "atoi padding hex string"
        assert string_at(b) == "1ab"," atoi incorrect no-zero hex padding"

    def test_auto_with_upper(self):
        upper_hex = bc.itohex(self.c, self.s, c_int(9), c_int(1))
        assert string_at(upper_hex) == "1AB", \
                " atoi incorrect no-zero upper hex padding"

def suite_blibc():
    bosTestSuite = unittest.makeSuite(BOSTest_atoi, 'test')
    return bosTestSuite

def main():
    suite1 = suite_blibc()
    alltests = unittest.TestSuite((suite1))
    runner = unittest.TextTestRunner()
    runner.run(alltests);

if __name__ == "__main__":
    main()
