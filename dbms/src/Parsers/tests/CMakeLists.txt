# Copyright 2023 PingCAP, Inc.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include_directories (${CMAKE_CURRENT_BINARY_DIR})

set(SRCS )

add_executable (lexer lexer.cpp ${SRCS})
target_link_libraries (lexer tiflash_parsers)

add_executable (select_parser select_parser.cpp ${SRCS})
target_link_libraries (select_parser tiflash_parsers)

add_executable (create_parser create_parser.cpp ${SRCS})
target_link_libraries (create_parser tiflash_parsers)

