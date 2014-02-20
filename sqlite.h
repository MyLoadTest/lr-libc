// There is really no reason to use the clunky Virtual Table Server. Use SQLite for lower volume situations, and MySQL for higher volume situations.
// http://en.wikipedia.org/wiki/SQLite
// http://www.sqlite.org/
// SQLite will work over a network filesystem, but because of the latency associated with most network filesystems, performance will not be great.

