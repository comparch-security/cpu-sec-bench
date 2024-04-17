import igraph as ig
import matplotlib.pyplot as plt
import json as js
import networkx as nx

ig.config["plotting.backend"] = "matplotlib"

with open(r"./configure.json", 'r') as input_file:
    data = js.load(input_file)

cases_dict = {}
cases_list = []
cases_category = []
edges_list = []
edges_category = []
color_dict = {"acc":"","mss":"", "mts":"","cpi":"", "cfi-b":"", "cfi-f":""}
color_tuple = ("#4E79A7","#F28E2B","#59A14F","#F1CE63","#E15759","#D4A6C8")
for key,color in zip(color_dict.keys(),color_tuple):
    color_dict[key] = color
cases_dependency_dict = {}

def custom_sort_key(key):
    """
    A function that custom sorts keys based on a predefined order. 
    It extracts the prefix from the key and handles a specific case for 'cfi'. 
    Returns the order of the prefix according to the predefined order.
    """
    # Define the order of prefixes
    prefix_order = {
        'acc': 1,
        'mss': 2,
        'mts': 3,
        'cpi': 4,
        'cfi': 5
    }
    
    # Extract the prefix from the key
    prefix = key.split('-')[0]
    middix = key.split('-')[1]
    if prefix == 'cfi' and middix != 'return':
        return 6
    else:
        return prefix_order[prefix]

data = dict(sorted(data.items(),key=lambda item: custom_sort_key(item[0])))

# build the mapping between index and case name
for (index,case_name) in enumerate(data):
    cases_category_str = case_name.split("-")[0]
    if cases_category_str == "cfi":
        cfi_case_category = case_name.split("-")[1]
        if cfi_case_category == "return":
            cases_category_str = "cfi-b"
        else:
            cases_category_str = "cfi-f"
    cases_category.append(cases_category_str)
    cases_dict[case_name] = index
    cases_dependency_dict[index] = ""
    index+=1
    cases_list.append(case_name)

# parse each case's require
for case_name in data:
    requirer_index = cases_dict[case_name]
    relaxer_index = specialer_index = requirer_index
    value = data[case_name]
    # print("The key and value are ({}) = ({})".format(key, value))
    if "require" in value: #{'0': [['acc-get-ra-offset-v-p-g1', 'mss-write-by-enclosing-array-index-overflow-stack']], '1': [['mss-write-by-enclosing-array-index-overflow-stack']], '2': [['mss-write-by-enclosing-array-index-underflow-stack']]
        require_cases = value["require"]
        dep_numbers = []
        if "require-comment" not in value:
            if len(require_cases) == 1 and len(list(require_cases.values())[0]) == 0:
                #print("continue")
                continue
            else:
                exit(1)
        require_index = 0
        for (ind,retry_case) in enumerate(require_cases): # [['acc-get-ra-offset-v-p-g1', 'mss-write-by-enclosing-array-index-overflow-stack']]
            oncetry_cases = require_cases[retry_case]
            if ind != 0:
                dep_numbers.append('||')
            if len(require_cases) != 1 and len(oncetry_cases) != 1:
                dep_numbers.append('(')
            for (ind,list_cases) in enumerate(oncetry_cases):
                if ind != 0:
                    dep_numbers.append(' & ')
                if len(oncetry_cases) != 1 and len(list_cases )!= 1:
                    dep_numbers.append('(')
                for cases in list_cases:
                    requiree_index = cases_dict[cases]
                    dep_numbers.append(requiree_index)
                    dep_numbers.append(' | ')
                    edges_list.append((requiree_index, requirer_index))
                dep_numbers.pop()
                if len(oncetry_cases) != 1 and len(list_cases )!= 1:
                    dep_numbers.append(')')
            if len(require_cases) != 1 and len(oncetry_cases) != 1:
                dep_numbers.append(')')
        cases_dependency_dict[requirer_index] = dep_numbers#[1:-1]

    if "require-comment" in value:
        cases = value["require-comment"]
        for item in cases:
            if "utilization" in item:
                edges_category.append("utilization")
            if "relaxation" in item:
                edges_category.append("relaxation")

csv_file = "map_data.csv"
with open(csv_file, mode='w', newline='', encoding='utf-8') as file:
    for (k,v) in cases_dependency_dict.items():
        tmp = ""
        case_name = ""
        for i in v:
            tmp = tmp + str(i)
            if type(i) == type(0):
                case_name = case_name + cases_list[i] + " "
        file.write("{},{},{}\n".format(k,cases_list[k],tmp))

# edges_set  = set(edges_list)
# edges_list = list(edges_set)

# for edge in edges_list:
#     print("source is {}, target is {}".format(cases_list[edge[0]],cases_list[edge[1]]))

g = ig.Graph(n=len(cases_list), edges=edges_list, directed = True)

g.vs["name"] = cases_list
g.vs["category"] = cases_category
g.es["category"] = edges_category
g.vs["fill"] = [ color_dict[c] for c in g.vs["category"]]
g.es["fill"] = ["#404040" for c in g.es["category"]]
# g.vs["gender"] = ["f", "m", "f", "m", "f", "m", "m"]
# g.es["is_formal"] = [False, False, True, True, True, False, True, False, False]
#g.vs["label"] = g.vs["name"]
g.write_gml("1.gml")

visual_style = {}
#visual_style["layout"] = g.layout_umap()
#visual_style["layout"] = g.layout("mds")
#visual_style["layout"] = g.layout("graphopt")
# visual_style["layout"] = g.layout_drl()
# visual_style["layout"] = g.layout("fr")
# visual_style["layout"] = g.layout("dh")
# visual_style["layout"] = g.layout_star(center=1)
# visual_style["layout"] = g.layout_reingold_tilford_circular()
# visual_style["layout"] = g.layout_reingold_tilford()
## visual_style["layout"] = g.layout_kamada_kawai()
# visual_style["layout"] = g.layout_fruchterman_reingold()

#visual_style["mark_groups"] = True
visual_style["vertex_size"] = 3
visual_style["vertex_frame_width"] = 1
visual_style["vertex_color"] = [color_dict[c] for c in g.vs["category"]]
visual_style["vertex_frame_color"] = [color_dict[c] for c in g.vs["category"]]

visual_style["edge_width"] = [0.5]
visual_style["edge_curved"] = 0
visual_style["edge_color"] = "grey"
visual_style["edge_arrow_size"] = 3
visual_style["edge_arrow_width"] = 1.5
visual_style["edge_style"] = "dashed"

# visual_style["vertex_label"] = g.vs["name"]
# visual_style["vertex_label_size"] = 7
ig.plot(g, target='myfile.pdf',**visual_style )
# G = g.to_networkx()
# nx.draw(G,style=['--'])
# plt.show()