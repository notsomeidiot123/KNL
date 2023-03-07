var num1;

64 num1 $;
scan num1;
var num2;

num1 @ num2 $;
print ""
64 @ num2 $;
print num2;
print "Printed out again: "
64 num1 $;
print num1;



// note to self: scope will keep increaseing every goto.
//another note to self: shadowing is a thing
//PUT IT IN THE DOCS
//if a variable of the same name is declared in a higher scope
//the variable in the higher scope will be referenced instead :/
