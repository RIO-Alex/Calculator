"""Run a command against a disposable, socket-only PostgreSQL cluster."""
import os
from pathlib import Path
import subprocess
import sys
import tempfile


def main():
    bindir = Path(sys.argv[1])
    with tempfile.TemporaryDirectory(prefix="calculator-pg-") as directory:
        root = Path(directory)
        data = root / "data"
        env = os.environ.copy()
        # Do not inherit settings pointing to the user's database or service.
        for key in list(env):
            if key.startswith("PG"):
                del env[key]
        env.update(PGHOST=str(root), PGPORT="5432", PGUSER="calculator_test",
                   PGDATABASE="postgres", PGCONNECT_TIMEOUT="5")
        subprocess.run([str(bindir / "initdb"), "-D", str(data), "-U",
                        "calculator_test", "-A", "trust", "--no-locale"],
                       env=env, check=True, stdout=subprocess.DEVNULL)
        started = False
        try:
            subprocess.run([str(bindir / "pg_ctl"), "-D", str(data), "-l",
                            str(root / "postgres.log"), "-o",
                            f"-F -k {root} -c listen_addresses=''", "-w", "start"],
                           env=env, check=True, stdout=subprocess.DEVNULL)
            started = True
            return subprocess.run(sys.argv[2:], env=env).returncode
        finally:
            if started:
                subprocess.run([str(bindir / "pg_ctl"), "-D", str(data),
                                "-m", "immediate", "-w", "stop"],
                               env=env, check=True, stdout=subprocess.DEVNULL)


if __name__ == "__main__":
    sys.exit(main())
