What is it
----------

The SAG module is used to create the embeddings of graphs, and to render them.

In general, this works by mapping the nodes of a graph to the 
cells of a RogueViz-supported tessellation or honeycomb. The number of
usable cells is limited (that is, a fixed region is set in advance for the embedding).

Simulated Annealing is used to find the 'optimal' mapping. The function optimized
depends on the method. Currently, there are three main 'methods' implemented:

NEAREST: we minimize the sum of w * d over all edges, where w is the edge weight,
and d is the distance between the endpoints of that edge. In other words, 
we want to place all nodes all close as possible, especially if the node weights are
big. See the following visualizations as an example of visualizations obtained using
this method:

https://www.youtube.com/watch?v=mDG3_f8R2Ns (SAG boardgames)
https://www.youtube.com/watch?v=WSyygk_3j9o (SAG roguelikes)
https://www.youtube.com/watch?v=HWQkDkeEUeM (SAG programming languages)

MATCH: we minimize the sum of squares of (d - a/w - b), where w and d are as above.
In other words, we want the distance between nodes to represent 1/w as well as possible;
a and b are scaling parameters.

(no examples for now)

LIKELIHOOD: this method is based on the Hyperbolic Random Graph model. According to
that model, each pair of nodes in distance d are connected with probability
1/(1+\exp((d-R)/T)). We maximize the likelihood, i.e., the product of these probabilities
for actual edges, and their complements for non-edges. In other words, nodes connected
with edges want to be close, while nodes not connected with edges want to be distant.

The following embeddings have been obtained using this method:

https://youtu.be/GQKaKF_yOL4 (brain connectomes)

The rest of this README details how to use SAG.

Cells
-----

Not yet documented

Graph
-----

Just use `-sag-weighted` to read a weighted graph (in format `node1;node2;weight`), 
or `-sag-unweighted` to read an unweighted graph (in format `node1 node2`).
