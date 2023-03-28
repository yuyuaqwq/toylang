# 说明

旨在学习编译原理所开发的编译器/虚拟机。

# 代码完善中...

# 文章
手把手教你开发ToyLang系列：
- https://yuyuaqwq.github.io/posts/c4d3.html

# EBNF
```EBNF
src = {stat}
stat = ifStat | forStat | whileStat | continueStat | breakStat | returnStat | newVarStat | assignStat | [exp] ';'
parList = [ ident {',' ident} ]
block = '{' {stat} '}'
ifStat = 'if' exp block [elStat]
elStat = {'elif' exp block} ['else' exp block]
forStat = 'for' ident ':' exp block
whileStat = 'while' exp block
continueStat = 'continue' ';'
breakStat = 'break' ';'
returnStat = 'return' exp ';'
newVarStat = ident ':=' exp ';'
assignStat = ident '=' exp ';'

exp = exp3
exp3 = exp2 { cmpOp exp2 }
exp2 = exp1 { arithOp2 exp1 }
exp1 = exp0 { arithOp1 exp0 }
cmpOp =  '!=' | '==' | '>' | '<' | '>=' | '<='
arithOp2 = '+' | '-'
arithOp1 = | '*' | '/'
exp0 = nullExp | boolExp | stringExp | numberExp | ident | funcCallExp | funcDefExp
funcDefExp = 'func' ident '(' parList ')' block
nullExp = 'null'
boolExp = 'false' | 'true'
stringExp = string
numberExp = number
funcCallExp = ident '(' expList ')'
expList = { exp [',' exp] }
```


# 虚拟机指令集
``` ToyByteCode
变长指令集，v表示变量，k表示常量
opcode占1字节，使用低7位，高1位保留
stop
nop
pushv vi
pushk ki 
popv vi
pop
add
sub
mul
div
call vi
ret
ne
eq
gt
ge
lt
le
jcf pc
jmp pc
```
