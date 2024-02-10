# Page Replacement Algorithms

This repository provides implementation for 4 Page Replacement Algorithms for Virtual Memory Management:

1. First-In-First-Out (FIFO) Page Replacement Algorithm:
   The FIFO algorithm replaces the oldest page in memory, based on the order of arrival.

2. Optimal (OPT) Page Replacement Algorithm:
   The OPT algorithm selects the page for eviction that will not be used for the longest future period, requiring knowledge of future references.

3. Least Recently Used (LRU) Page Replacement Algorithm:
   The LRU algorithm replaces the least recently used page, based on the principle that pages not recently accessed are less likely to be needed soon.

4. Second Chance (CLOCK) Page Replacement Algorithm:
   The CLOCK algorithm is an enhancement of FIFO that gives a second chance to pages that have been referenced recently, mimicking a circular queue.
