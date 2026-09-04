# The AI
## Formalizing the Problem
To construct an efficient AI chess bot we can utilise the MinMax algorithm where:
- Given a current board state
- Given whose turn it is
- Given all possible moves the player can make
- Construct all possible subsequent board states
- Recursively evaluate all boards based upon:
  - A base case board evaluation function to statically evaluate a board position (`depth = 0`)
  - A recursive case whereby the board's evaluation is determined based upon the best move which the opposing player can make in response
- Make the AI bot play the move with the highest evaluation.

From this, we can formalise the AI bot as:

```
  data Board = [[Maybe Pieces]]

  ai :: Board -> Board
  ai =  select . minMax . tree
```

Where:
- `tree :: Board -> Tree Board` constructs an n-arity game tree of all possible board states from the given board.
- `minMax :: Tree Board -> Tree (Int, Board)` assigning a MinMax evaluation of each board state in the tree
- `select :: Tree (Int, Board) -> Board` selects the highest evaluation board at depth 1

## Static Evaluation
But how do we statically evaluate a given board state? I.e. how do we evaluate a board state, if the game is abandoned at that state.

This is a cruical question to pose. MinMax AIs for simpler games, we can simply evaluate if a given position as won/undecided/lost and always select a random path to a maximum value. 

However, the same cannot be applied to a Chess AI, primarily because the game is far more nuanced than simply won/undecided/lost; a given board state may not be forced mate at a reasonable depth, but may be heavily favorable for one player or another. This concept is prevalent for simpler games but less relevant and not necessary to produce a good AI.

We must defined an evaluation function:
```
eval :: Board -> Int
```
I propose it considers the following characteristics:
1. Combined total piece material
2. Pawn structure (connected pawns, central pawns)
3. Pinned pieces
4. Strength of secondary pieces (the amount of squares the rooks and bishops are attacking)

Note that the most important characteristic is the sum of the total material difference between players. The other characteristics are secondary. Trivially, a checkmate is $\infin$ value. 

## Prospective Optimizations
There are three primary points of optimization I believe will be necessary:
1. Concurrently processing the MinMax of the game tree - using multiple threads
2. Alpha-Beta pruning to reduce the search space of the tree
3. Heuristic-based searching to disregard portions of the game tree which are trivially bad decisions - e.g. ones which lead to a forced mate (against the AI), hanging a high value piece.

*Note: it will be challenging to correctly implement heuristic-based searching as the difference between hanging  a piece and sacrificing a piece (for more value) is intrinsically determined by the subsequent moves. Hence, if we do not evaulate these subsequent moves, we will not know if the piece is hung, or sacrificed.*