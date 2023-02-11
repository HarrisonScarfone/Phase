results = [["g2g4", 41],
["e1d1", 42],
["e1f1", 41],
["d5e6", 45],
["g2h3", 42],
["e5c6", 41],
["e5c4", 41],
["e5g4", 43],
["e5d3", 42],
["e5d7", 44],
["e5f7", 44],
["e5g6", 41],
["c3b5", 38],
["c3a4", 41],
["c3b1", 41],
["c3d1", 41],
["d2h6", 40],
["d2g5", 41],
["d2f4", 42],
["d2e3", 42],
["d2c1", 42],
["e2a6", 36],
["e2b5", 38],
["e2c4", 40],
["e2d3", 41],
["e2d1", 43],
["e2f1", 42],
["a1b1", 42],
["a1c1", 42],
["a1d1", 42],
["h1f1", 41],
["h1g1", 41],
["f3f6", 38],
["f3h3", 42],
["f3f5", 44],
["f3h5", 42],
["f3f4", 42],
["f3g4", 42],
["f3d3", 41],
["f3e3", 42],
["f3g3", 42],
["e1g1", 41],
["e1c1", 42],
]

results2 = [["a2a3", 44],
["b2b3", 42],
["g2g3", 42],
["d5d6", 41],
["a2a4", 44],
["g2g4", 42],
["g2h3", 43],
["d5e6", 46],
["c3b1", 42],
["c3d1", 42],
["c3a4", 42],
["c3b5", 39],
["e5d3", 43],
["e5c4", 42],
["e5g4", 44],
["e5c6", 41],
["e5g6", 42],
["e5d7", 45],
["e5f7", 44],
["d2c1", 43],
["d2e3", 43],
["d2f4", 43],
["d2g5", 42],
["d2h6", 41],
["e2d1", 44],
["e2f1", 44],
["e2d3", 42],
["e2c4", 41],
["e2b5", 39],
["e2a6", 36],
["a1b1", 43],
["a1c1", 43],
["a1d1", 43],
["h1f1", 43],
["h1g1", 43],
["f3d3", 42],
["f3e3", 43],
["f3g3", 43],
["f3h3", 43],
["f3f4", 43],
["f3g4", 43],
["f3f5", 45],
["f3h5", 43],
["f3f6", 39],
["e1d1", 43],
["e1f1", 43],
["e1g1", 43],
["e1c1", 43]
]

sorted_results = sorted(results)
sorted_results2 = sorted(results2)

for result, result2 in zip(sorted_results, sorted_results2):
  print(f"{result[0]}: {result[1]} | {result2[1]}")