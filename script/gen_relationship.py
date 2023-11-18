import igraph as ig
import matplotlib.pyplot as plt

ig.config["plotting.backend"] = "matplotlib"

g = ig.Graph([(0,1), (0,2), (2,3), (3,4), (4,2), (2,5), (5,0), (6,3), (5,6)])

g.vs["name"] = ["Alice", "Bob", "Claire", "Dennis", "Esther", "Frank", "George"]
g.vs["age"] = [25, 31, 18, 47, 22, 23, 50]
g.vs["gender"] = ["f", "m", "f", "m", "f", "m", "m"]
g.es["is_formal"] = [False, False, True, True, True, False, True, False, False]

g.vs["label"] = g.vs["name"]
layout= g.layout("kk")
ig.plot(g, layout=layout)
plt.show()
#plt.savefig("demo.png")