在多线程环境中,如果在signal处理函数中调用localtime函数,可能导致线程死锁.

因此,将glibc中的localtime的主要实现函数__offtime独立出来，这样就不需要调用系统的localtime，避免了线程死锁。

