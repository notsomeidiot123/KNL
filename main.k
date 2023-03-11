//calculate Fibonacci Sequence

var first;
var second;
var i;
//initialization of variables
1 first $;
0 second $;
0 i $;
//print the first variable
print first;
print "\n";
then
	label loop; //this is the 26 at the bottom of the stack
end
then
	var tmp;
	first @ tmp $;
	first @ second @ +; print;
	print "\n";
	first $;
	tmp @ second $;
end
i @ 1 + i $;
if i @ 45 > then
	goto loop;
end
else then
	print "Finished!\n";
end
var file;