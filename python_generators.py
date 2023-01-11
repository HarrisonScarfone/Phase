# COORDINATE TO UINT_64T LOOKUP TABLE GENERATOR
# ---------------------------------------------------------------------------------
# FILES = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']
# RANKS = [8, 7, 6, 5, 4, 3, 2, 1]

# create_toggled_64_bit_int = lambda index: 1 << index

# for i, rank in enumerate(RANKS):
#   for j, file in enumerate(FILES):
#     print("{" + f'\"{file}{rank}\", {create_toggled_64_bit_int(i * 8 + j)}' "},")

# ---------------------------------------------------------------------------------

# UINT_64T TO COORDINATE LOOKUP TABLE GENERATOR
# ---------------------------------------------------------------------------------
FILES = ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h']
RANKS = [8, 7, 6, 5, 4, 3, 2, 1]

create_toggled_64_bit_int = lambda index: 1 << index

for i, rank in enumerate(RANKS):
  for j, file in enumerate(FILES):
    print("{" + f'{create_toggled_64_bit_int(i * 8 + j)}, \"{file}{rank}\"' "},")

# ---------------------------------------------------------------------------------