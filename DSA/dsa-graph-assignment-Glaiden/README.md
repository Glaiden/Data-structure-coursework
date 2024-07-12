[![Review Assignment Due Date](https://classroom.github.com/assets/deadline-readme-button-24ddc0f5d75046c5622901739e7c5dd533143b0c8e959d652212380cedb1ea36.svg)](https://classroom.github.com/a/A4rc3hix)
[![Open in Codespaces](https://classroom.github.com/assets/launch-codespace-7f7980b617ed060a017424585567c406b6ee15c891e84e1186181d67ecf80aa0.svg)](https://classroom.github.com/open-in-codespaces?assignment_repo_id=12540802)
# Graph assignment

This assignment aims to familiarize you with graph representations in the form of an adjacency list graph (ALGraph) abstract data type (ADT). We will also practice using the graph data structure for a famous associated algorithm, Dijkstra's algorithm.

Note that the term "list" in "Adjacency List" is not interpreted as a traditional linked list. We will be using std::vec to implement the "list". This is because we want to optimize for maintaining some form of order in the adjacency lists and array-based implementations allow us to perform this more efficiently.

As usual most of the work you need to do should be in a new [ALGraph.cpp](ALGraph.cpp) file, that conforms to the class provided in the [ALGraph.h](ALGraph.h) file.

The [test.cpp](test.cpp) file contains the tests that your implementation will be tested against. Your goal is to make all the tests pass.

You are again advised to approach the assignment in the same test-driven development (TDD) manner as with previous assignments.

Here's the usual next section that tells you how to compile and run the tests. 

# Compilation and Testing

Everything is done via the [Makefile](Makefile) in the terminal. 

In this assignment, there is a new target pattern specified in the Makefile to create a png file that allows you to visually inspect the output graphs:

```
make test<test_number>-dot
```

For example, to generate the graph0.png file for test0, run:

```
make test0-dot
```

The remaining commands are the same as before.

To compile the code, run:

```
make
```

To run the tests, run:

```
make test<test_number>
```

Replace test_number with the correct one you are currently working on. We recommend that you start with test0 and work your way up.
For example, to run the first test, run:

```
make test1
```

To run the tests without comparing to the expected output, for example for test1, run:

```
make test1-nocompare
```

To clean up the compiled files, run:

```
make clean
```

# Description

As usual, the combination of the header file and the test file should give you a good idea of what you need to do to address our requirements. However, here are some pointers that you may find useful.

## Adding edges

There are two types of edges that can be used to construct the graph:
- `addDEdge` will add a new directed edge to the graph
- `addUEdge` will hence add a new undirected edge to the graph

As an undirected edge is simply made up of two opposing directed edges, `addUEdge` should simply use `addDEdge` to avoid duplication.

Adding directed edge (_source_, _destination_, _weight_) translates to adding a new `AdjInfo` with _destination_ and _weight_ to the list (vec) at the index _source_. This implements the concept of an adjacency list to represent graphs where each `AdjInfo` represents a entry in the list. Note again that we are not using the traditional "linked list" notion of an adjacency list. 

You should also implement the adjacency lists such that the edges in each vertex's vector are sorted by weight from smallest to largest. If two edges have the same weight, then sort by the vertex ID.

## Viewing the graph

The graphs are basically defined via their adjacency lists. The `dumpGraph` method is provided to allow you to print the adjacency lists in a human readable form.

For example `test0` contains the following output:

```
ID: [ 1] -- 10 --> [ 2] -- 20 --> [ 3]
ID: [ 2] -- 10 --> [ 1] -- 30 --> [ 3]
ID: [ 3] -- 20 --> [ 1] -- 30 --> [ 2]
```

Here, we can see that the graph has 3 vertices, with IDs 1, 2, and 3. The arrows indicate the direction of the edges with the weight as the label in the middle. For example, vertex 1 has an outgoing edge to vertex 2 with weight 10, and an outgoing edge to vertex 3 with weight 20. Vertex 2 has an outgoing edge to vertex 1 with weight 10, and an outgoing edge to vertex 3 with weight 30. And so on...

The graphs are also dumped into a file called `graph<test-number>.dot` in the DOT format. You can use this file to visualize the graph using Graphviz. For example, you can run the following command to generate a PNG file with the graph0.dot generated in `test0`:

```
dot -Tpng graph0.dot -o graph0.png
```

Of course, you will need to install Graphviz first. Installation instructions can be found at https://graphviz.org/download/ .

You can also use the online Graphviz editor at https://dreampuf.github.io/GraphvizOnline/ to visualize the graph by pasting in (the web form) the textual contents of the `.dot` files.

## Dijkstra's algorithm

The method named `dijkstra` unsurprising implements the common Dijkstra's all-source shortest paths algorithm. It's a const method to ensure you do not alter the structure of the tree when running this algorithm. The return value of this method is a vector of `DijkstraInfo`, each containing:
- total `cost` of the shortest path to the respective node represented
- the shortest `path` stored as a list (vector) of node IDs

An essential data structure that you have to use is the **priority queue**, normally implemented using the heap data structure. This is used to _pop_ the next node with the lowest total cost. You can use any existing API to accomplish this.

## Debugging

There is also the usual intrusive method `getAdjList` that allows the client to directly get access to the internal `adjList_`. Again this is usually bad practice in real world APIs but we do it here to allow the test.cpp tests to easily print the graph.

# Grading

This is the same blurb about how these assignments are meant to be formative, so the points you receive on passing the tests will not count towards your formal grades. However, the summative assessments (quizzes, practical test, etc.) will be based on these assignments, so it is in your best interest to complete them.
