# debug_cell

这是最简单的lock脚本。

使用的话主要有三步：

1. 先发deploy交易将lock脚本放到一个cell里面。
2. setup操作其实就是把资产从默认的lock脚本转到新的lock脚本。
3. 然后继续流转的过程中就可以一直使用新的lock脚本。

整个操作其实相当于在换签名算法。

# vote

同样要先deploy这个lock脚本。

然后投票的过程就类似上面的setup过程，即把资产从默认的lock脚本转到vote脚本。

---

最简单的设计是发起人先setup（data里面设置好候选项，初始票数都是0），然后后面每个人接力去投票（把自己想投的候选项的票数加一）。

因为每次投票都会把之前的cell销毁掉，所以是串行的。如果投票人多的话，就变自旋锁了。大家得一直轮询。

还有个问题是每次都要把老的cell销毁，把里面的数据拷贝出来。稍微改动一点（加一）之后生成一个新的cell。感觉有点浪费。

实现上也有个难题，A投完票之后，B不知道A新产生的cell是哪个？也没法从发起人创建的cell开始反向遍历。

如果能按照binary hash去查live cell会很有帮助。

---

现在实现的是一个并行的方案。

相当于每个投票的人都去做setup。data里面就只放自己的投票信息（候选人信息和lock脚本的信息，通过传统方式公开）。

最后有一个唱票人，把所有这些投票交易作为input消耗掉，在output的data里面放上统计的投票结果。

解决了前述方案，串行，复制数据的问题。

实现上有个类似的难题，最后唱票人还是无法独立搜索出所有的投票交易。

还有一个问题是单纯依靠参数，很难做等式守恒检查，基本上load tx是必须的。

但是这样就没动力去拆分lock和type了。反正已经能看到所有数据了，在一个脚本里做完所有的检查还更省事（cycle消耗也可能更少，不用重复load tx）。

而且现在的方案会检查多遍，虽然在这个场景里面放到type脚本能解决问题，但是如果output也有多个，同样是检查多遍。

---

todo:

1. 候选项要可配置。
2. 唱票人做成多签。
3. 投票人也可以增加白名单。这三项信息作为lock脚本的配套数据，单独放一个cell，通过deps引用？
4. 增加时间限制，多少块之内可以投票，唱票在这之后，而且也得在限定的块数内完成。怎么实现？