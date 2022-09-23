#!/usr/bin/env python3
"""Test communication with x52d and verify that the behavior is as expected"""
# pylint: disable=consider-using-f-string

import glob
import os
import os.path
import platform
import shlex
import signal
import subprocess
import tempfile
import time
import sys

class TestCase:
    """TestCase class handles an individual test case"""
    def __init__(self, data):
        """Create a new test case"""
        self.desc = None
        self.in_cmd = None
        self.exp_resp = None
        self.parse(data)

    def parse(self, data):
        """Parses a string of the following form:
           <description>
           space separated input line, possibly quoted
           space separated expected response, possibly quoted
        """
        self.desc, in_cmd, exp_resp = data.splitlines()
        self.in_cmd = shlex.split(in_cmd)
        self.exp_resp = '\0'.join(shlex.split(exp_resp)).encode() + b'\0'

    def execute(self, index, suite):
        """Execute the test case and return the result in TAP format"""
        def dump_failed(name, value):
            """Dump the failed test case"""
            print("# {}".format(name))
            for argv in value.decode().split('\0'):
                print("#\t {}".format(argv))
            print()

        def print_result(passed):
            """Print the test case result and description"""
            out = "ok {} - {}".format(index+1, self.desc)
            if not passed:
                out = "not " + out
            print(out)

        cmd = [suite.find_control_program(),
               '-s', suite.command, '--',
               *self.in_cmd]

        testcase = subprocess.run(cmd, stdout=subprocess.PIPE, check=False)
        if testcase.returncode != 0:
            print_result(False)
            print("# x52ctl returned code: {}".format(testcase.returncode))
            dump_failed("Expected", self.exp_resp)
            dump_failed("Got", testcase.stdout)
        elif testcase.stdout != self.exp_resp:
            print_result(False)
            dump_failed("Expected", self.exp_resp)
            dump_failed("Got", testcase.stdout)
        else:
            print_result(True)


class Test:
    """Test class runs a series of unit tests"""

    def __init__(self):
        """Create a new instance of the Test class"""
        self.program = self.find_daemon_program()
        self.tmpdir = tempfile.TemporaryDirectory() # pylint: disable=consider-using-with
        self.command = os.path.join(self.tmpdir.name, "x52d.cmd")
        self.notify = os.path.join(self.tmpdir.name, "x52d.notify")
        self.daemon = None
        self.testcases = []

    def __enter__(self):
        """Context manager entry"""
        self.launch_daemon()
        return self

    def __exit__(self, *exc):
        """Context manager exit"""
        self.terminate_daemon()
        self.tmpdir.cleanup()

    @staticmethod
    def find_daemon_program():
        """Find the daemon program. This script should be run from the
           root of the build directory"""
        daemon_candidates = glob.glob('**/x52d', recursive=True)
        if not daemon_candidates:
            print("Bail out! Unable to find X52 daemon.")
            sys.exit(1)

        return os.path.realpath(daemon_candidates[0])

    @staticmethod
    def find_control_program():
        """Find the control program. This script should be run from the
           root of the build directory"""
        ctl_candidates = glob.glob('**/x52ctl', recursive=True)
        if not ctl_candidates:
            print("Bail out! Unable to find x52ctl.")
            sys.exit(1)

        return os.path.realpath(ctl_candidates[0])

    def launch_daemon(self):
        """Launch an instance of the running daemon"""
        if self.daemon is not None:
            # We've already started the daemon, check if it is still running
            if self.daemon.poll() is None:
                return

            self.daemon = None

        daemon_cmdline = [
            self.program,
            "-f", # Run in foreground
            "-q", # Quiet logging
            "-c", os.path.join(self.tmpdir.name, "x52d.cfg"), # Default config file
            "-l", os.path.join(self.tmpdir.name, "x52d.log"), # Output logs to log file
            "-p", os.path.join(self.tmpdir.name, "x52d.pid"), # PID file
            "-s", self.command, # Command socket path
            "-b", self.notify, # Notification socket path
        ]

        # Create empty config file
        with open(daemon_cmdline[4], 'w', encoding='utf-8'):
            pass

        self.daemon = subprocess.Popen(daemon_cmdline) # pylint: disable=consider-using-with

        print("# Sleeping 2 seconds for daemon to start")
        time.sleep(2)

    def terminate_daemon(self):
        """Terminate a running daemon"""
        if self.daemon is None:
            return

        # Send a SIGTERM to the daemon
        os.kill(self.daemon.pid, signal.SIGTERM)
        try:
            self.daemon.wait(timeout=15)
        except subprocess.TimeoutExpired:
            # Forcibly kill the running process
            self.daemon.kill()
        finally:
            self.daemon = None

    def append(self, testcase):
        """Add one testcase to the test case list"""
        self.testcases.append(testcase)

    def extend(self, testcases):
        """Add one or more testcases to the test case list"""
        self.testcases.extend(testcases)

    @staticmethod
    def dump_failed(name, value):
        """Dump the failed test case"""
        print("# {}".format(name))
        for argv in value.decode().split('\0'):
            print("#\t {}".format(argv))
        print()

    def run_tests(self):
        """Run test cases"""
        print("1..{}".format(len(self.testcases)))
        for index, testcase in enumerate(self.testcases):
            testcase.execute(index, self)

    def find_and_parse_testcase_files(self):
        """Find and parse *.tc files"""
        basedir = os.path.dirname(os.path.realpath(__file__))
        pattern = os.path.join(basedir, 'tests', '**', '*.tc')
        tc_files = sorted(glob.glob(pattern, recursive=True))

        for tc_file in tc_files:
            with open(tc_file, encoding='utf-8') as tc_fd:
                # Test cases are separated by blank lines
                testcases = tc_fd.read().split('\n\n')
                self.extend(TestCase(tc_data) for tc_data in testcases)

def main():
    """Main routine adds test cases to the Test class and runs them"""
    # Only run the tests on Linux platform
    if platform.system() != 'Linux':
        print('1..0 # Skipping tests on', platform.system())
        return

    with Test() as test:
        test.find_and_parse_testcase_files()
        test.run_tests()

if __name__ == '__main__':
    main()
