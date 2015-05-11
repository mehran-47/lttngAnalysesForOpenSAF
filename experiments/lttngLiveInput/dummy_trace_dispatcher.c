#include <stdio.h>
#include <lttng/tracef.h>

int main(){
    tracef("Dummy trace-point with number %d", 42);
}