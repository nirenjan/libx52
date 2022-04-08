#!/usr/bin/env python3
"""Test communication with x52d and verify that the behavior is as expected"""

import glob
import os
import os.path
import shlex
import signal
import socket
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
        self.in_cmd = '\0'.join(shlex.split(in_cmd)).encode() + b'\0'
        self.exp_resp = '\0'.join(shlex.split(exp_resp)).encode() + b'\0'

    def execute(self, index, cmdsock):
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

        cmdsock.send(self.in_cmd)
        got_resp = cmdsock.recv(1024)

        if got_resp != self.exp_resp:
            print_result(False)
            dump_failed("Expected", self.exp_resp)
            dump_failed("Got", got_resp)
        else:
            print_result(True)


class Test:
    """Test class runs a series of unit tests"""

    def __init__(self):
        """Create a new instance of the Test class"""
        self.program = self.find_daemon_program()
        self.tmpdir = tempfile.TemporaryDirectory()
        self.command = os.path.join(self.tmpdir.name, "x52d.cmd")
        self.cmdsock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
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
            "-v", # Verbose logging
            "-c", os.path.join(self.tmpdir.name, "x52d.cfg"), # Default config file
            "-l", os.path.join(self.tmpdir.name, "x52d.log"), # Output logs to log file
            "-p", os.path.join(self.tmpdir.name, "x52d.pid"), # PID file
            "-s", self.command, # Command socket path
        ]

        # Create empty config file
        with open(daemon_cmdline[4], 'w'):
            pass

        self.daemon = subprocess.Popen(daemon_cmdline)

        print("# Sleeping 2 seconds for daemon to start")
        time.sleep(2)

        self.cmdsock.connect(self.command)

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
            self.cmdsock.close()

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
            testcase.execute(index, self.cmdsock)

    def find_and_parse_testcase_files(self):
        """Find and parse *.tc files"""
        basedir = os.path.dirname(os.path.realpath(__file__))
        pattern = os.path.join(basedir, '**', 'comm', '*.tc')
        tc_files = glob.glob(pattern, recursive=True)

        for tc_file in tc_files:
            with open(tc_file) as tc_fd:
                # Test cases are separated by blank lines
                testcases = tc_fd.read().split('\n\n')
                self.extend(TestCase(tc_data) for tc_data in testcases)

def main():
    """Main routine adds test cases to the Test class and runs them"""
    with Test() as test:
        test.find_and_parse_testcase_files()
        test.run_tests()

if __name__ == '__main__':
    main()
