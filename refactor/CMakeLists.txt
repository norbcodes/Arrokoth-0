# Arrokoth-0 - A simple, basic programming language.
# Copyright (C) 2026  NorbCodes
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 3.0.0 FATAL_ERROR)

project(
    "Arrokoth-0"
    VERSION 1.0.0
    DESCRIPTION "A simple, basic programming language as a learning project."
    LANGUAGES C
)

set(
    ARROKOTH_SRC
    ${CMAKE_SOURCE_DIR}/src/graph.c
    ${CMAKE_SOURCE_DIR}/src/codegen.c
    ${CMAKE_SOURCE_DIR}/src/frontend.c
    ${CMAKE_SOURCE_DIR}/src/lexer.c
    ${CMAKE_SOURCE_DIR}/src/parser.c
    ${CMAKE_SOURCE_DIR}/src/ast.c
)

add_executable(arrokoth ${ARROKOTH_SRC})
target_include_directories(arrokoth PRIVATE "${CMAKE_SOURCE_DIR}/include")
target_compile_features(arrokoth PRIVATE c_std_17)
target_compile_options(arrokoth PRIVATE -m64 -Wall -Wextra -g3)

# target_compile_options(arrokoth PRIVATE -m64 -Wall -Wextra -g3 -ggdb3 -fsanitize=address)
# target_link_options(arrokoth PRIVATE -fsanitize=address)

