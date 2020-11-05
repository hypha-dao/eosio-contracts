# Graph definition

![Alt text](https://g.gravizo.com/svg?
  digraph G {
    aize ="4,4";
    root_node [shape=box];
    root_node -> parse [weight=8];
    parse -> execute;
    root_node -> init [style=dotted];
    root_node -> cleanup;
    execute -> { make_string; printf}
    init -> make_string;
    edge [color=red];
    root_node -> printf [style=bold,label="100 times"];
    make_string [label="make a string"];
    node [shape=box,style=filled,color=".7 .3 1.0"];
    execute -> compare;
  }
)


## other example

![Alt text](https://g.gravizo.com/source/custom_mark10?https%3A%2F%2Fraw.githubusercontent.com%hypha-dao%2Feosio-contracts%2Fnative-ballots%2Fdocs%2Fgraph.md?2)

<details> 
<summary></summary>
custom_mark10
  digraph G {
    size ="4,4";
    root_node [shape=box];
    root_node -> parse [weight=8];
    parse -> execute;
    root_node -> init [style=dotted];
    root_node -> cleanup;
    execute -> { make_string; printf};
    init -> make_string;
    edge [color=red];
    root_node -> printf [style=bold,label="100 times"];
    make_string [label="make a string"];
    node [shape=box,style=filled,color=".7 .3 1.0"];
    execute -> compare;
  }
custom_mark10
</details>


![Alt text](https://g.gravizo.com/source/svg/custom_mark12?https%3A%2F%2Fraw.githubusercontent.com%hypha-dao%2Feosio-contracts%2Fnative-ballots%2Fdocs%2Fgraph.md?3)


<details> 
<summary></summary>
custom_mark12
/**
*Structural Things
*@opt commentname
*@note Notes can
*be extended to
*span multiple lines
*/
class Structural{}

/**
*@opt all
*@note Class
*/
class Counter extends Structural {
        static public int counter;
        public int getCounter();
}

/**
*@opt shape activeclass
*@opt all
*@note Active Class
*/
class RunningCounter extends Counter{}
custom_mark12
</details>
