import igraph as ig
import matplotlib.pyplot as plt
import json as js

ig.config["plotting.backend"] = "matplotlib"

with open(r"./configure.json", 'r') as input_file:
    data = js.load(input_file)

cases_dict = {}
cases_list = []
cases_category = []
color_dict = {"mss":"#9e0142", "mts":"#f46d43", "acc":"#fee08b", "cpi":"#e6f598", "cfi":"#66c2a5"}
edges_list = []
special_edges_list = []
relax_edges_list   = []

# build the mapping between index and case name
for (index,case_name) in enumerate(data):
    cases_category.append(case_name.split("-")[0])
    cases_dict[case_name] = index
    index+=1
    cases_list.append(case_name)

# print the keys and values
for case_name in data:
    requirer_index = cases_dict[case_name]
    relaxer_index = specialer_index = requirer_index
    value = data[case_name]
    # print("The key and value are ({}) = ({})".format(key, value))
    if "require" in value: #{'0': [['acc-get-ra-offset-v-p-g1', 'mss-write-by-enclosing-array-index-overflow-stack']], '1': [['mss-write-by-enclosing-array-index-overflow-stack']], '2': [['mss-write-by-enclosing-array-index-underflow-stack']]
        require_cases = value["require"]
        for retry_case in require_cases: # [['acc-get-ra-offset-v-p-g1', 'mss-write-by-enclosing-array-index-overflow-stack']]
            oncetry_cases = require_cases[retry_case]
            for list_cases in oncetry_cases:
                for cases in list_cases:
                    requiree_index = cases_dict[cases]
                    edges_list.append((requiree_index, requirer_index))

    if "specialization" in value:
        cases = value["specialization"]
        for case in cases:
            specialee_index = cases_dict[case]
            special_edges_list.append((specialer_index, specialee_index))
    if "relaxation" in value:
        cases = value["relaxation"]
        for case in cases:
            relaxee_index = cases_dict[case]
            relax_edges_list.append((relaxer_index, relaxee_index))

edges_set  = set(edges_list)
edges_list = list(edges_set)

edges_category = []

require_len = len(edges_list)
special_len = len(special_edges_list)
relax_len   = len(relax_edges_list)
all_len = require_len + special_len + relax_len

for ind in range(all_len):
    if(ind < require_len):
        edges_category.append("require")
    elif(ind >= require_len and ind < all_len - relax_len):
        edges_category.append("specialization")
    else:
        edges_category.append("relaxation")

edges_list.extend(special_edges_list)
edges_list.extend(relax_edges_list)

for edge in edges_list:
    print("source is {}, target is {}".format(cases_list[edge[0]],cases_list[edge[1]]))

g = ig.Graph(n=len(cases_list), edges=edges_list, directed = True)

g.vs["name"] = cases_list
g.vs["category"] = cases_category
g.es["category"] = edges_category
# g.vs["gender"] = ["f", "m", "f", "m", "f", "m", "m"]
# g.es["is_formal"] = [False, False, True, True, True, False, True, False, False]

#g.vs["label"] = g.vs["name"]
layout= g.layout("fr")

visual_style = {}
visual_style["layout"] = layout
visual_style["vertex_color"] = [color_dict[c] for c in g.vs["category"]]
visual_style["edge_width"] = [0.5]
visual_style["curved"]     = 10
visual_style["vertex_size"] = 10
# visual_style["vertex_label"] = g.vs["name"]
# visual_style["vertex_label_size"] = 7
g.write_gml("1.gml")