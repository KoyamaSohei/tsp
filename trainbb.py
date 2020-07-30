import optuna
import subprocess
import os
import sys


def objective(trial):
    tmax = trial.suggest_uniform('x', 1000, 1000000)
    delta = trial.suggest_uniform('y', 0.6, 0.98)
    res = 0
    for k in range(10,37):
        cmd = "./bb.out -n random/v" + str(k) + ".tsp"
        os.environ["TMAX"] = str(tmax)
        os.environ["DELTA"] = str(delta)
        d = subprocess.check_output(cmd.split())
        res += int(d)
    return res

study = optuna.load_study(study_name='trainbb', storage='sqlite:///trainbb.db')
study.optimize(objective, n_trials=100)
print(study.best_params)