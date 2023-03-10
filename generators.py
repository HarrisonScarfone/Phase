results = [["b4b3", 4622747],
["e6e5", 3547303],
["g6g5", 3842623],
["c7c6", 4228757],
["d7d6", 3943388],
["c7c5", 3768997],
["h3g2", 4952538],
["e6d5", 4260720],
["b4c3", 4254867],
["b6a4", 3901986],
["b6c4", 3970261],
["b6d5", 3755248],
["b6c8", 3238488],
["f6e4", 5731451],
["f6g4", 4701027],
["f6d5", 4412716],
["f6h5", 4249355],
["f6h7", 3890845],
["f6g8", 3942954],
["a6e2", 3434041],
["a6d3", 4068257],
["a6c4", 4124935],
["a6b5", 4263111],
["a6b7", 4166477],
["a6c8", 3281538],
["g7h6", 4114821],
["g7f8", 3446399],
["a8b8", 4415332],
["a8c8", 3903665],
["a8d8", 4008542],
["h8h4", 4232005],
["h8h5", 4257739],
["h8h6", 3618612],
["h8h7", 3656735],
["h8f8", 3145229],
["h8g8", 3304883],
["e7c5", 5225052],
["e7d6", 4591394],
["e7f7", 3290690],
["e7d8", 3627718],
["e7f8", 3587357],
["e8f7", 3283126],
["e8g8", 3861082],
["e8f8", 3945744],
]


results2 = [["c7c6", 4228830],
["c7c5", 3769067],
["d7d6", 3943458],
["e6e5", 3547372],
["g6g5", 3842694],
["b4b3", 4622822],
["e8f8", 3945744],
["e8f7", 3283126],
["e6d5", 4260793],
["b4c3", 4254940],
["h3g2", 4952611],
["b6c8", 3238555],
["b6a4", 3902057],
["b6c4", 3970332],
["b6d5", 3755318],
["f6g8", 3942956],
["f6h7", 3890888],
["f6h5", 4249399],
["f6g4", 4701073],
["f6d5", 4412759],
["f6e4", 5731501],
["g7f8", 3446401],
["g7h6", 4114858],
["a6e2", 3434109],
["a6c8", 3281605],
["a6b7", 4166549],
["a6b5", 4263184],
["a6c4", 4125007],
["a6d3", 4068329],
["a8b8", 4415406],
["a8c8", 3903736],
["a8d8", 4008614],
["h8f8", 3145229],
["h8g8", 3304883],
["h8h7", 3656735],
["h8h6", 3618612],
["h8h5", 4257739],
["h8h4", 4232005],
["e7f7", 3290690],
["e7d8", 3627788],
["e7f8", 3587402],
["e7d6", 4591468],
["e7c5", 5225129],
["e8g8", 3861082],]

sorted_results = sorted(results)
sorted_results2 = sorted(results2)

print(len(sorted_results), len(sorted_results2))

for result, result2 in zip(sorted_results, sorted_results2):
  print(f"{result[0]}: {result[1]} | {result2[1]}{'NOT EQUAL' if result[1] != result2[1] else ''}")