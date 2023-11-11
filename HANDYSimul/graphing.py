import pandas as pd
import matplotlib.pyplot as plt

# TODO
# run cpp executable here
# read in csv file
# plot graphs

TARGET_FILE = "output/"
sim_df = pd.read_csv('HANDYSimul/output/simulation.csv')

# COLUMNS
TIME = sim_df['timestep']
ELITES = sim_df['elites']
COMMONERS = sim_df['commoners']
NATURE = sim_df['nature']
WEALTH = sim_df['wealth']

figure, axis = plt.subplots(2, 2)

# Subplots
axis[0, 0].plot(TIME, ELITES, label="elites", color='red')
axis[0, 0].set_title("elites")
axis[0, 0].set_ylabel("population")
axis[0, 0].set_xlabel("timestep")

axis[0, 1].plot(TIME, COMMONERS, label="commoners", color='blue')
axis[0, 1].set_title("commoners")
axis[0, 1].set_ylabel("population")
axis[0, 1].set_xlabel("timestep")

axis[1, 0].plot(TIME, NATURE, label="nature", color='green')
axis[1, 0].set_title("nature")
axis[1, 0].set_ylabel("stock")
axis[1, 0].set_xlabel("timestep")

axis[1, 1].plot(TIME, WEALTH, label="wealth", color='orange')
axis[1, 1].set_title("wealth")
axis[1, 1].set_ylabel("stock")
axis[1, 1].set_xlabel("timestep")

figure.tight_layout()
figure.savefig("HANDYSimul/output/graphs.png")

