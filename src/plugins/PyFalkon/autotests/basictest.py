import KClient
import unittest

class BasicTest(unittest.TestCase):

    def test_module_version(self):
        self.assertEqual(KClient.__version__.count('.'), 2)
        self.assertIsNotNone(KClient.registerPlugin)

    def test_mainapplication(self):
        self.assertIsNotNone(KClient.MainApplication.instance())

    def test_create_window(self):
        window = KClient.MainApplication.instance().createWindow(KClient.Qz.BW_NewWindow)
        self.assertIsNotNone(window)


suite = unittest.defaultTestLoader.loadTestsFromTestCase(BasicTest)
if unittest.TextTestRunner().run(suite).failures:
    raise(Exception("FAIL"))
