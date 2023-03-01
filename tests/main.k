var num1;
var num2;
5 num1 $;
label test
// note to self: scope will keep increaseing every goto.
//another note to self: shadowing is a thing
//PUT IT IN THE DOCS
//if a variable of the same name is declared in a higher scope
//the variable in the higher scope will be referenced instead :/
print "Hello, World!"
1 num1 @ - num1 $;
if 0 num1 @ = ! then
	print num1
	print "Jump test"
	goto test
end
print "Hello Again!"