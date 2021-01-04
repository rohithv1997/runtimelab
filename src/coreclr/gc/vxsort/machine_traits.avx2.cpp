// Licensed to the .NET Foundation under one or more agreements.
// The .NET Foundation licenses this file to you under the MIT license.

#include "common.h"
#include "machine_traits.avx2.h"

namespace vxsort {

alignas(128) const int8_t perm_table_64[128] = {
        0, 1, 2, 3, 4, 5, 6, 7,  // 0b0000 (0)
        2, 3, 4, 5, 6, 7, 0, 1,  // 0b0001 (1)
        0, 1, 4, 5, 6, 7, 2, 3,  // 0b0010 (2)
        4, 5, 6, 7, 0, 1, 2, 3,  // 0b0011 (3)
        0, 1, 2, 3, 6, 7, 4, 5,  // 0b0100 (4)
        2, 3, 6, 7, 0, 1, 4, 5,  // 0b0101 (5)
        0, 1, 6, 7, 2, 3, 4, 5,  // 0b0110 (6)
        6, 7, 0, 1, 2, 3, 4, 5,  // 0b0111 (7)
        0, 1, 2, 3, 4, 5, 6, 7,  // 0b1000 (8)
        2, 3, 4, 5, 0, 1, 6, 7,  // 0b1001 (9)
        0, 1, 4, 5, 2, 3, 6, 7,  // 0b1010 (10)
        4, 5, 0, 1, 2, 3, 6, 7,  // 0b1011 (11)
        0, 1, 2, 3, 4, 5, 6, 7,  // 0b1100 (12)
        2, 3, 0, 1, 4, 5, 6, 7,  // 0b1101 (13)
        0, 1, 2, 3, 4, 5, 6, 7,  // 0b1110 (14)
        0, 1, 2, 3, 4, 5, 6, 7,  // 0b1111 (15)
};

alignas(2048) const int8_t perm_table_32[2048] = {
    0, 1, 2, 3, 4, 5, 6, 7, // 0b00000000 (0)
    1, 2, 3, 4, 5, 6, 7, 0, // 0b00000001 (1)
    0, 2, 3, 4, 5, 6, 7, 1, // 0b00000010 (2)
    2, 3, 4, 5, 6, 7, 0, 1, // 0b00000011 (3)
    0, 1, 3, 4, 5, 6, 7, 2, // 0b00000100 (4)
    1, 3, 4, 5, 6, 7, 0, 2, // 0b00000101 (5)
    0, 3, 4, 5, 6, 7, 1, 2, // 0b00000110 (6)
    3, 4, 5, 6, 7, 0, 1, 2, // 0b00000111 (7)
    0, 1, 2, 4, 5, 6, 7, 3, // 0b00001000 (8)
    1, 2, 4, 5, 6, 7, 0, 3, // 0b00001001 (9)
    0, 2, 4, 5, 6, 7, 1, 3, // 0b00001010 (10)
    2, 4, 5, 6, 7, 0, 1, 3, // 0b00001011 (11)
    0, 1, 4, 5, 6, 7, 2, 3, // 0b00001100 (12)
    1, 4, 5, 6, 7, 0, 2, 3, // 0b00001101 (13)
    0, 4, 5, 6, 7, 1, 2, 3, // 0b00001110 (14)
    4, 5, 6, 7, 0, 1, 2, 3, // 0b00001111 (15)
    0, 1, 2, 3, 5, 6, 7, 4, // 0b00010000 (16)
    1, 2, 3, 5, 6, 7, 0, 4, // 0b00010001 (17)
    0, 2, 3, 5, 6, 7, 1, 4, // 0b00010010 (18)
    2, 3, 5, 6, 7, 0, 1, 4, // 0b00010011 (19)
    0, 1, 3, 5, 6, 7, 2, 4, // 0b00010100 (20)
    1, 3, 5, 6, 7, 0, 2, 4, // 0b00010101 (21)
    0, 3, 5, 6, 7, 1, 2, 4, // 0b00010110 (22)
    3, 5, 6, 7, 0, 1, 2, 4, // 0b00010111 (23)
    0, 1, 2, 5, 6, 7, 3, 4, // 0b00011000 (24)
    1, 2, 5, 6, 7, 0, 3, 4, // 0b00011001 (25)
    0, 2, 5, 6, 7, 1, 3, 4, // 0b00011010 (26)
    2, 5, 6, 7, 0, 1, 3, 4, // 0b00011011 (27)
    0, 1, 5, 6, 7, 2, 3, 4, // 0b00011100 (28)
    1, 5, 6, 7, 0, 2, 3, 4, // 0b00011101 (29)
    0, 5, 6, 7, 1, 2, 3, 4, // 0b00011110 (30)
    5, 6, 7, 0, 1, 2, 3, 4, // 0b00011111 (31)
    0, 1, 2, 3, 4, 6, 7, 5, // 0b00100000 (32)
    1, 2, 3, 4, 6, 7, 0, 5, // 0b00100001 (33)
    0, 2, 3, 4, 6, 7, 1, 5, // 0b00100010 (34)
    2, 3, 4, 6, 7, 0, 1, 5, // 0b00100011 (35)
    0, 1, 3, 4, 6, 7, 2, 5, // 0b00100100 (36)
    1, 3, 4, 6, 7, 0, 2, 5, // 0b00100101 (37)
    0, 3, 4, 6, 7, 1, 2, 5, // 0b00100110 (38)
    3, 4, 6, 7, 0, 1, 2, 5, // 0b00100111 (39)
    0, 1, 2, 4, 6, 7, 3, 5, // 0b00101000 (40)
    1, 2, 4, 6, 7, 0, 3, 5, // 0b00101001 (41)
    0, 2, 4, 6, 7, 1, 3, 5, // 0b00101010 (42)
    2, 4, 6, 7, 0, 1, 3, 5, // 0b00101011 (43)
    0, 1, 4, 6, 7, 2, 3, 5, // 0b00101100 (44)
    1, 4, 6, 7, 0, 2, 3, 5, // 0b00101101 (45)
    0, 4, 6, 7, 1, 2, 3, 5, // 0b00101110 (46)
    4, 6, 7, 0, 1, 2, 3, 5, // 0b00101111 (47)
    0, 1, 2, 3, 6, 7, 4, 5, // 0b00110000 (48)
    1, 2, 3, 6, 7, 0, 4, 5, // 0b00110001 (49)
    0, 2, 3, 6, 7, 1, 4, 5, // 0b00110010 (50)
    2, 3, 6, 7, 0, 1, 4, 5, // 0b00110011 (51)
    0, 1, 3, 6, 7, 2, 4, 5, // 0b00110100 (52)
    1, 3, 6, 7, 0, 2, 4, 5, // 0b00110101 (53)
    0, 3, 6, 7, 1, 2, 4, 5, // 0b00110110 (54)
    3, 6, 7, 0, 1, 2, 4, 5, // 0b00110111 (55)
    0, 1, 2, 6, 7, 3, 4, 5, // 0b00111000 (56)
    1, 2, 6, 7, 0, 3, 4, 5, // 0b00111001 (57)
    0, 2, 6, 7, 1, 3, 4, 5, // 0b00111010 (58)
    2, 6, 7, 0, 1, 3, 4, 5, // 0b00111011 (59)
    0, 1, 6, 7, 2, 3, 4, 5, // 0b00111100 (60)
    1, 6, 7, 0, 2, 3, 4, 5, // 0b00111101 (61)
    0, 6, 7, 1, 2, 3, 4, 5, // 0b00111110 (62)
    6, 7, 0, 1, 2, 3, 4, 5, // 0b00111111 (63)
    0, 1, 2, 3, 4, 5, 7, 6, // 0b01000000 (64)
    1, 2, 3, 4, 5, 7, 0, 6, // 0b01000001 (65)
    0, 2, 3, 4, 5, 7, 1, 6, // 0b01000010 (66)
    2, 3, 4, 5, 7, 0, 1, 6, // 0b01000011 (67)
    0, 1, 3, 4, 5, 7, 2, 6, // 0b01000100 (68)
    1, 3, 4, 5, 7, 0, 2, 6, // 0b01000101 (69)
    0, 3, 4, 5, 7, 1, 2, 6, // 0b01000110 (70)
    3, 4, 5, 7, 0, 1, 2, 6, // 0b01000111 (71)
    0, 1, 2, 4, 5, 7, 3, 6, // 0b01001000 (72)
    1, 2, 4, 5, 7, 0, 3, 6, // 0b01001001 (73)
    0, 2, 4, 5, 7, 1, 3, 6, // 0b01001010 (74)
    2, 4, 5, 7, 0, 1, 3, 6, // 0b01001011 (75)
    0, 1, 4, 5, 7, 2, 3, 6, // 0b01001100 (76)
    1, 4, 5, 7, 0, 2, 3, 6, // 0b01001101 (77)
    0, 4, 5, 7, 1, 2, 3, 6, // 0b01001110 (78)
    4, 5, 7, 0, 1, 2, 3, 6, // 0b01001111 (79)
    0, 1, 2, 3, 5, 7, 4, 6, // 0b01010000 (80)
    1, 2, 3, 5, 7, 0, 4, 6, // 0b01010001 (81)
    0, 2, 3, 5, 7, 1, 4, 6, // 0b01010010 (82)
    2, 3, 5, 7, 0, 1, 4, 6, // 0b01010011 (83)
    0, 1, 3, 5, 7, 2, 4, 6, // 0b01010100 (84)
    1, 3, 5, 7, 0, 2, 4, 6, // 0b01010101 (85)
    0, 3, 5, 7, 1, 2, 4, 6, // 0b01010110 (86)
    3, 5, 7, 0, 1, 2, 4, 6, // 0b01010111 (87)
    0, 1, 2, 5, 7, 3, 4, 6, // 0b01011000 (88)
    1, 2, 5, 7, 0, 3, 4, 6, // 0b01011001 (89)
    0, 2, 5, 7, 1, 3, 4, 6, // 0b01011010 (90)
    2, 5, 7, 0, 1, 3, 4, 6, // 0b01011011 (91)
    0, 1, 5, 7, 2, 3, 4, 6, // 0b01011100 (92)
    1, 5, 7, 0, 2, 3, 4, 6, // 0b01011101 (93)
    0, 5, 7, 1, 2, 3, 4, 6, // 0b01011110 (94)
    5, 7, 0, 1, 2, 3, 4, 6, // 0b01011111 (95)
    0, 1, 2, 3, 4, 7, 5, 6, // 0b01100000 (96)
    1, 2, 3, 4, 7, 0, 5, 6, // 0b01100001 (97)
    0, 2, 3, 4, 7, 1, 5, 6, // 0b01100010 (98)
    2, 3, 4, 7, 0, 1, 5, 6, // 0b01100011 (99)
    0, 1, 3, 4, 7, 2, 5, 6, // 0b01100100 (100)
    1, 3, 4, 7, 0, 2, 5, 6, // 0b01100101 (101)
    0, 3, 4, 7, 1, 2, 5, 6, // 0b01100110 (102)
    3, 4, 7, 0, 1, 2, 5, 6, // 0b01100111 (103)
    0, 1, 2, 4, 7, 3, 5, 6, // 0b01101000 (104)
    1, 2, 4, 7, 0, 3, 5, 6, // 0b01101001 (105)
    0, 2, 4, 7, 1, 3, 5, 6, // 0b01101010 (106)
    2, 4, 7, 0, 1, 3, 5, 6, // 0b01101011 (107)
    0, 1, 4, 7, 2, 3, 5, 6, // 0b01101100 (108)
    1, 4, 7, 0, 2, 3, 5, 6, // 0b01101101 (109)
    0, 4, 7, 1, 2, 3, 5, 6, // 0b01101110 (110)
    4, 7, 0, 1, 2, 3, 5, 6, // 0b01101111 (111)
    0, 1, 2, 3, 7, 4, 5, 6, // 0b01110000 (112)
    1, 2, 3, 7, 0, 4, 5, 6, // 0b01110001 (113)
    0, 2, 3, 7, 1, 4, 5, 6, // 0b01110010 (114)
    2, 3, 7, 0, 1, 4, 5, 6, // 0b01110011 (115)
    0, 1, 3, 7, 2, 4, 5, 6, // 0b01110100 (116)
    1, 3, 7, 0, 2, 4, 5, 6, // 0b01110101 (117)
    0, 3, 7, 1, 2, 4, 5, 6, // 0b01110110 (118)
    3, 7, 0, 1, 2, 4, 5, 6, // 0b01110111 (119)
    0, 1, 2, 7, 3, 4, 5, 6, // 0b01111000 (120)
    1, 2, 7, 0, 3, 4, 5, 6, // 0b01111001 (121)
    0, 2, 7, 1, 3, 4, 5, 6, // 0b01111010 (122)
    2, 7, 0, 1, 3, 4, 5, 6, // 0b01111011 (123)
    0, 1, 7, 2, 3, 4, 5, 6, // 0b01111100 (124)
    1, 7, 0, 2, 3, 4, 5, 6, // 0b01111101 (125)
    0, 7, 1, 2, 3, 4, 5, 6, // 0b01111110 (126)
    7, 0, 1, 2, 3, 4, 5, 6, // 0b01111111 (127)
    0, 1, 2, 3, 4, 5, 6, 7, // 0b10000000 (128)
    1, 2, 3, 4, 5, 6, 0, 7, // 0b10000001 (129)
    0, 2, 3, 4, 5, 6, 1, 7, // 0b10000010 (130)
    2, 3, 4, 5, 6, 0, 1, 7, // 0b10000011 (131)
    0, 1, 3, 4, 5, 6, 2, 7, // 0b10000100 (132)
    1, 3, 4, 5, 6, 0, 2, 7, // 0b10000101 (133)
    0, 3, 4, 5, 6, 1, 2, 7, // 0b10000110 (134)
    3, 4, 5, 6, 0, 1, 2, 7, // 0b10000111 (135)
    0, 1, 2, 4, 5, 6, 3, 7, // 0b10001000 (136)
    1, 2, 4, 5, 6, 0, 3, 7, // 0b10001001 (137)
    0, 2, 4, 5, 6, 1, 3, 7, // 0b10001010 (138)
    2, 4, 5, 6, 0, 1, 3, 7, // 0b10001011 (139)
    0, 1, 4, 5, 6, 2, 3, 7, // 0b10001100 (140)
    1, 4, 5, 6, 0, 2, 3, 7, // 0b10001101 (141)
    0, 4, 5, 6, 1, 2, 3, 7, // 0b10001110 (142)
    4, 5, 6, 0, 1, 2, 3, 7, // 0b10001111 (143)
    0, 1, 2, 3, 5, 6, 4, 7, // 0b10010000 (144)
    1, 2, 3, 5, 6, 0, 4, 7, // 0b10010001 (145)
    0, 2, 3, 5, 6, 1, 4, 7, // 0b10010010 (146)
    2, 3, 5, 6, 0, 1, 4, 7, // 0b10010011 (147)
    0, 1, 3, 5, 6, 2, 4, 7, // 0b10010100 (148)
    1, 3, 5, 6, 0, 2, 4, 7, // 0b10010101 (149)
    0, 3, 5, 6, 1, 2, 4, 7, // 0b10010110 (150)
    3, 5, 6, 0, 1, 2, 4, 7, // 0b10010111 (151)
    0, 1, 2, 5, 6, 3, 4, 7, // 0b10011000 (152)
    1, 2, 5, 6, 0, 3, 4, 7, // 0b10011001 (153)
    0, 2, 5, 6, 1, 3, 4, 7, // 0b10011010 (154)
    2, 5, 6, 0, 1, 3, 4, 7, // 0b10011011 (155)
    0, 1, 5, 6, 2, 3, 4, 7, // 0b10011100 (156)
    1, 5, 6, 0, 2, 3, 4, 7, // 0b10011101 (157)
    0, 5, 6, 1, 2, 3, 4, 7, // 0b10011110 (158)
    5, 6, 0, 1, 2, 3, 4, 7, // 0b10011111 (159)
    0, 1, 2, 3, 4, 6, 5, 7, // 0b10100000 (160)
    1, 2, 3, 4, 6, 0, 5, 7, // 0b10100001 (161)
    0, 2, 3, 4, 6, 1, 5, 7, // 0b10100010 (162)
    2, 3, 4, 6, 0, 1, 5, 7, // 0b10100011 (163)
    0, 1, 3, 4, 6, 2, 5, 7, // 0b10100100 (164)
    1, 3, 4, 6, 0, 2, 5, 7, // 0b10100101 (165)
    0, 3, 4, 6, 1, 2, 5, 7, // 0b10100110 (166)
    3, 4, 6, 0, 1, 2, 5, 7, // 0b10100111 (167)
    0, 1, 2, 4, 6, 3, 5, 7, // 0b10101000 (168)
    1, 2, 4, 6, 0, 3, 5, 7, // 0b10101001 (169)
    0, 2, 4, 6, 1, 3, 5, 7, // 0b10101010 (170)
    2, 4, 6, 0, 1, 3, 5, 7, // 0b10101011 (171)
    0, 1, 4, 6, 2, 3, 5, 7, // 0b10101100 (172)
    1, 4, 6, 0, 2, 3, 5, 7, // 0b10101101 (173)
    0, 4, 6, 1, 2, 3, 5, 7, // 0b10101110 (174)
    4, 6, 0, 1, 2, 3, 5, 7, // 0b10101111 (175)
    0, 1, 2, 3, 6, 4, 5, 7, // 0b10110000 (176)
    1, 2, 3, 6, 0, 4, 5, 7, // 0b10110001 (177)
    0, 2, 3, 6, 1, 4, 5, 7, // 0b10110010 (178)
    2, 3, 6, 0, 1, 4, 5, 7, // 0b10110011 (179)
    0, 1, 3, 6, 2, 4, 5, 7, // 0b10110100 (180)
    1, 3, 6, 0, 2, 4, 5, 7, // 0b10110101 (181)
    0, 3, 6, 1, 2, 4, 5, 7, // 0b10110110 (182)
    3, 6, 0, 1, 2, 4, 5, 7, // 0b10110111 (183)
    0, 1, 2, 6, 3, 4, 5, 7, // 0b10111000 (184)
    1, 2, 6, 0, 3, 4, 5, 7, // 0b10111001 (185)
    0, 2, 6, 1, 3, 4, 5, 7, // 0b10111010 (186)
    2, 6, 0, 1, 3, 4, 5, 7, // 0b10111011 (187)
    0, 1, 6, 2, 3, 4, 5, 7, // 0b10111100 (188)
    1, 6, 0, 2, 3, 4, 5, 7, // 0b10111101 (189)
    0, 6, 1, 2, 3, 4, 5, 7, // 0b10111110 (190)
    6, 0, 1, 2, 3, 4, 5, 7, // 0b10111111 (191)
    0, 1, 2, 3, 4, 5, 6, 7, // 0b11000000 (192)
    1, 2, 3, 4, 5, 0, 6, 7, // 0b11000001 (193)
    0, 2, 3, 4, 5, 1, 6, 7, // 0b11000010 (194)
    2, 3, 4, 5, 0, 1, 6, 7, // 0b11000011 (195)
    0, 1, 3, 4, 5, 2, 6, 7, // 0b11000100 (196)
    1, 3, 4, 5, 0, 2, 6, 7, // 0b11000101 (197)
    0, 3, 4, 5, 1, 2, 6, 7, // 0b11000110 (198)
    3, 4, 5, 0, 1, 2, 6, 7, // 0b11000111 (199)
    0, 1, 2, 4, 5, 3, 6, 7, // 0b11001000 (200)
    1, 2, 4, 5, 0, 3, 6, 7, // 0b11001001 (201)
    0, 2, 4, 5, 1, 3, 6, 7, // 0b11001010 (202)
    2, 4, 5, 0, 1, 3, 6, 7, // 0b11001011 (203)
    0, 1, 4, 5, 2, 3, 6, 7, // 0b11001100 (204)
    1, 4, 5, 0, 2, 3, 6, 7, // 0b11001101 (205)
    0, 4, 5, 1, 2, 3, 6, 7, // 0b11001110 (206)
    4, 5, 0, 1, 2, 3, 6, 7, // 0b11001111 (207)
    0, 1, 2, 3, 5, 4, 6, 7, // 0b11010000 (208)
    1, 2, 3, 5, 0, 4, 6, 7, // 0b11010001 (209)
    0, 2, 3, 5, 1, 4, 6, 7, // 0b11010010 (210)
    2, 3, 5, 0, 1, 4, 6, 7, // 0b11010011 (211)
    0, 1, 3, 5, 2, 4, 6, 7, // 0b11010100 (212)
    1, 3, 5, 0, 2, 4, 6, 7, // 0b11010101 (213)
    0, 3, 5, 1, 2, 4, 6, 7, // 0b11010110 (214)
    3, 5, 0, 1, 2, 4, 6, 7, // 0b11010111 (215)
    0, 1, 2, 5, 3, 4, 6, 7, // 0b11011000 (216)
    1, 2, 5, 0, 3, 4, 6, 7, // 0b11011001 (217)
    0, 2, 5, 1, 3, 4, 6, 7, // 0b11011010 (218)
    2, 5, 0, 1, 3, 4, 6, 7, // 0b11011011 (219)
    0, 1, 5, 2, 3, 4, 6, 7, // 0b11011100 (220)
    1, 5, 0, 2, 3, 4, 6, 7, // 0b11011101 (221)
    0, 5, 1, 2, 3, 4, 6, 7, // 0b11011110 (222)
    5, 0, 1, 2, 3, 4, 6, 7, // 0b11011111 (223)
    0, 1, 2, 3, 4, 5, 6, 7, // 0b11100000 (224)
    1, 2, 3, 4, 0, 5, 6, 7, // 0b11100001 (225)
    0, 2, 3, 4, 1, 5, 6, 7, // 0b11100010 (226)
    2, 3, 4, 0, 1, 5, 6, 7, // 0b11100011 (227)
    0, 1, 3, 4, 2, 5, 6, 7, // 0b11100100 (228)
    1, 3, 4, 0, 2, 5, 6, 7, // 0b11100101 (229)
    0, 3, 4, 1, 2, 5, 6, 7, // 0b11100110 (230)
    3, 4, 0, 1, 2, 5, 6, 7, // 0b11100111 (231)
    0, 1, 2, 4, 3, 5, 6, 7, // 0b11101000 (232)
    1, 2, 4, 0, 3, 5, 6, 7, // 0b11101001 (233)
    0, 2, 4, 1, 3, 5, 6, 7, // 0b11101010 (234)
    2, 4, 0, 1, 3, 5, 6, 7, // 0b11101011 (235)
    0, 1, 4, 2, 3, 5, 6, 7, // 0b11101100 (236)
    1, 4, 0, 2, 3, 5, 6, 7, // 0b11101101 (237)
    0, 4, 1, 2, 3, 5, 6, 7, // 0b11101110 (238)
    4, 0, 1, 2, 3, 5, 6, 7, // 0b11101111 (239)
    0, 1, 2, 3, 4, 5, 6, 7, // 0b11110000 (240)
    1, 2, 3, 0, 4, 5, 6, 7, // 0b11110001 (241)
    0, 2, 3, 1, 4, 5, 6, 7, // 0b11110010 (242)
    2, 3, 0, 1, 4, 5, 6, 7, // 0b11110011 (243)
    0, 1, 3, 2, 4, 5, 6, 7, // 0b11110100 (244)
    1, 3, 0, 2, 4, 5, 6, 7, // 0b11110101 (245)
    0, 3, 1, 2, 4, 5, 6, 7, // 0b11110110 (246)
    3, 0, 1, 2, 4, 5, 6, 7, // 0b11110111 (247)
    0, 1, 2, 3, 4, 5, 6, 7, // 0b11111000 (248)
    1, 2, 0, 3, 4, 5, 6, 7, // 0b11111001 (249)
    0, 2, 1, 3, 4, 5, 6, 7, // 0b11111010 (250)
    2, 0, 1, 3, 4, 5, 6, 7, // 0b11111011 (251)
    0, 1, 2, 3, 4, 5, 6, 7, // 0b11111100 (252)
    1, 0, 2, 3, 4, 5, 6, 7, // 0b11111101 (253)
    0, 1, 2, 3, 4, 5, 6, 7, // 0b11111110 (254)
    0, 1, 2, 3, 4, 5, 6, 7, // 0b11111111 (255)
};

}

