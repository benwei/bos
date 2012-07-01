import unittest
from ctypes import *

bc = CDLL("./libblibc.so")

verbose = 0

def boslog(msg):
    global verbose
    if verbose:
        print msg


class BOSTest(unittest.TestCase):
    def runTest(self):
        self.test_atoi();

    def test_atoi(self):
        '''
        const char *itohex(uint32_t c, char *s, int size, int upper)
        '''
        c = 427
        s = (c_byte*9)();
        b = bc.itohex(c, s, c_int(9), c_int(0))
        for i in range(0, 8):
            boslog("[%d]%c" % (i, s[i]))
        assert chr(s[7]) == 'b', 'incorrent data'

def suite_blibc():
    '''
    suite = unittest.makeSuite(bosTestCase,'test')
    '''
    bosTestSuite = unittest.TestSuite()
    testCase = BOSTest()
    bosTestSuite.addTest(testCase)
    return bosTestSuite


def main():
    suite1 = suite_blibc()
    alltests = unittest.TestSuite((suite1))
    runner = unittest.TextTestRunner()
    runner.run(alltests);

if __name__ == "__main__":
    main()
