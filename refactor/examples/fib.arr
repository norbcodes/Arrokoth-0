[ Fibonacci sequence calculator

program

[ Define our variables:
var F,S,I,Pos,EndPos;

proc Fib
{
    while Pos < EndPos
    {
        [ `I` is for intermediate result.
        I = F + S;
        F = S;
        S = I;
        Pos = Pos + 1;
    };
}

proc main
{
    F = 0; [ fib(0) = 0
    S = 1; [ fib(1) = 1
    I = 0;
    Pos = 0;
    EndPos = 15; [ Set this to the index of the fibonacci number you want - 1
    [ Setting endpos to 15 will give us the 16th Fibonacci number (987)
    run Fib;
    # S;
}

end
