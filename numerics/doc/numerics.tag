<?xml version='1.0' encoding='ISO-8859-1' standalone='yes' ?>
<tagfile>
  <compound kind="class">
    <name>AcceptEvent</name>
    <filename>classAcceptEvent.html</filename>
    <templarg>DataIter</templarg>
    <templarg>TimeIter</templarg>
    <member kind="function">
      <type></type>
      <name>AcceptEvent</name>
      <anchorfile>classAcceptEvent.html</anchorfile>
      <anchor>ff620b1aea0a4c1dc923f37d15cfc997</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~AcceptEvent</name>
      <anchorfile>classAcceptEvent.html</anchorfile>
      <anchor>35da18f0be0f1cef594f8a22228d325c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>checkEvent</name>
      <anchorfile>classAcceptEvent.html</anchorfile>
      <anchor>d1e842348e731ade386fb75eec86c22b</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, DataIter &amp;event, TimeIter &amp;eventtime, DataIter &amp;index, TimeIter &amp;indextime, DataIter &amp;prevevent, TimeIter &amp;prevtime, EventData &amp;outevents, double &amp;threshold, double &amp;minthresh, double &amp;maxthresh, double &amp;time, double &amp;size, double &amp;width)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>checkPeak</name>
      <anchorfile>classAcceptEvent.html</anchorfile>
      <anchor>ab15c65ab85fe0eb9df8f10826b6feea</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, DataIter &amp;event, TimeIter &amp;eventtime, DataIter &amp;index, TimeIter &amp;indextime, DataIter &amp;prevevent, TimeIter &amp;prevtime, EventList &amp;outevents, double &amp;threshold, double &amp;minthresh, double &amp;maxthresh, double &amp;time, double &amp;size, double &amp;width)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>checkTrough</name>
      <anchorfile>classAcceptEvent.html</anchorfile>
      <anchor>4182079965f0f805c8314f0bd884ee72</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, DataIter &amp;event, TimeIter &amp;eventtime, DataIter &amp;index, TimeIter &amp;indextime, DataIter &amp;prevevent, TimeIter &amp;prevtime, EventList &amp;outevents, double &amp;threshold, double &amp;minthresh, double &amp;maxthresh, double &amp;time, double &amp;size, double &amp;width)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Array</name>
    <filename>classArray.html</filename>
    <templarg>T</templarg>
    <member kind="typedef">
      <type>T</type>
      <name>value_type</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>f2b2079c4150cc7b0c8eec293f8142b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>T *</type>
      <name>pointer</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>6f8267bae40e009a9c42db733d93ea23</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>T &amp;</type>
      <name>reference</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>20619e1802c28caf3e0f0f88537e714d</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const T &amp;</type>
      <name>const_reference</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>06a437672069e1b07cd71edf2d3f5d22</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>int</type>
      <name>size_type</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>9e823a7deba3a411d938794422676f24</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>T *</type>
      <name>iterator</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>0dfcde1d43a98288773043f8f66d01fb</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const T *</type>
      <name>const_iterator</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>1ca1ca7b73aac0feec74a536f223e408</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Array</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>fcf6839187d300241a968d7c2d6d8d4c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Array</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>5bcbd234ea774368ad3cc366340234da</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Array</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>10157566c7def8f12b12ce966e4d13e6</anchor>
      <arglist>(int n, const T &amp;val)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Array</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>e15425748caae96a2746a56a7d5ef47a</anchor>
      <arglist>(const S *a, int n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Array</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>980226ee52576e446d8bd31e5d4e5daf</anchor>
      <arglist>(const T *a, int n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Array</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>5ada243627c3fb779cafdd98b21af1d9</anchor>
      <arglist>(const vector&lt; S &gt; &amp;a, int first=0, int last=-1)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Array</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>c94765015e73e1ccf7268c6238c91269</anchor>
      <arglist>(const Array&lt; S &gt; &amp;a, int first=0, int last=-1)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Array</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>34718169ec45392c6a8f377cfd8dd9dd</anchor>
      <arglist>(const Array&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~Array</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>7e30ae855d889f0e63c06cf008d07b74</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>26c27835d1c414b8245a4d08ac97010b</anchor>
      <arglist>(const S &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>198852a05dcbddc08763edd89b4c4a17</anchor>
      <arglist>(const Array&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>a1a78dfc9c52150d1cd394233c99d621</anchor>
      <arglist>(const S *a, int n)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>82f49a4231b0359be8b2f9d064e85349</anchor>
      <arglist>(const T *a, int n)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>35e8f9e119e6fe4c9484f45e7d95375d</anchor>
      <arglist>(const S &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>85e96ec80ea8dc9849ff0ed67dea6c62</anchor>
      <arglist>(const Array&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>zeros</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>bdb5364896db747850c72a16cdf981bd</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>ones</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>e07f5a948c32fa007f61eba1e7d50ac5</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>rand</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>2f789d575fdad1efe787e98d5478f8a5</anchor>
      <arglist>(int n, R &amp;r=numerics::rnd)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>randNorm</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>f1d73512e7046fd208c789147f3eaa7e</anchor>
      <arglist>(int n, R &amp;r=numerics::rnd)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>391b0706257432ddf1fb4af1242e2c3d</anchor>
      <arglist>(S *a, int n, const S &amp;val=0) const</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>7db4f5f003aced71875dd5f48b53453d</anchor>
      <arglist>(T *a, int n, const T &amp;val=0) const</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>b520e0f4dfec63a663e9fc583b218d53</anchor>
      <arglist>(S &amp;a) const</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>d9197578dddb4221938558c2e3283477</anchor>
      <arglist>(Array&lt; S &gt; &amp;a) const </arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>15b01199581fb61ccc64ef0af46a4c01</anchor>
      <arglist>(T a, int n=1)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>9d3cd53bcc637454fd51093c022aaaac</anchor>
      <arglist>(const S *a, int n)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>43355877c8293af00d1cf21edb80e491</anchor>
      <arglist>(const T *a, int n)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>4013672eab661968382c7c25e5a37ee7</anchor>
      <arglist>(const S &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>bc995c930c584bfdd5e3802bcab248fe</anchor>
      <arglist>(const Array&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>size</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>866e794ef2d5b6ade70524386ab03ac0</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>empty</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>a136c24a2ee3d9f9b34cb59b9a3490aa</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>resize</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>7f37ab6f07cea2e9fe50b3c8696be11a</anchor>
      <arglist>(int n, const T &amp;val=0)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>clear</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>c63c645cc58b3800e13802635680f20d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>capacity</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>ab2788f3ea839e59d6b06a66f34190a4</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>reserve</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>9ca51100c2fe76dde016fa7ccb07a7fb</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>free</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>75abea3807ae3af1c9330c23b2875f3a</anchor>
      <arglist>(int n=0)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>operator[]</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>475c7fe5f841eeb81ce348fb1cde27c5</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>operator[]</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>2446215b18fd39e7caad474899e34146</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>at</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>9db2f62ac3f322a9338b29d10d717060</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>at</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>8068ff80d458d38fd106cdd536441770</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>front</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>a0085ccc2b651801c98ffbfc3b1d0728</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>front</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>053045ee73cd47685192193e6ef5eace</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>back</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>74fec6263ea8f70d55b3c2b0a82dd937</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>back</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>4c125aec82a3426deff7b2460239871b</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>57a7f91f90e9d6a7511495eeb1534c6d</anchor>
      <arglist>(const T &amp;val)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>135f2b63eaf24755fa1eb5ea7b4e03f6</anchor>
      <arglist>(const R &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>pop</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>29b8b112ea1e6b5b83714b0d60e7ce4c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const T *</type>
      <name>data</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>8b3050751d1a81a9b8c74785093fd226</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>T *</type>
      <name>data</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>7dee172881f5348af6e5144efb5453c8</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>begin</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>f60162ca2a5bcc5f89c65ea79caca44e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>begin</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>783aae877ab910ba567b78fdeb8f55b2</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>end</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>f5413e2f2e74990e0b1018cb50a0c031</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>end</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>f12c5abda1e4f106c511e39a035143b0</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>insert</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>a0842310c4ea119a85fa5b59ca36d19d</anchor>
      <arglist>(int i, const T &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>insert</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>68e9f3f0fd3e0342c09af9d7bc3ed847</anchor>
      <arglist>(iterator i, const T &amp;v)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>erase</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>b747aeabea2c14c7cfdbd5c3969dac3f</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>erase</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>e65ecd9a43c4d3b305bf2f11934a3230</anchor>
      <arglist>(iterator i)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>sortedIndex</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>44a0f369465c2e788def78094c605eca</anchor>
      <arglist>(Array&lt; int &gt; &amp;indices, int first=0, int last=-1)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1SCALARDEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>26278b3db8c3b8e92501e2d88e9c90b0</anchor>
      <arglist>(Array&lt; T &gt;, operator=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1DEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>916cd0c314e39181f8dbd3aa3dee45f1</anchor>
      <arglist>(Array&lt; T &gt;, operator+=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1DEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>aa53c47690eb185da800e0a84df0de99</anchor>
      <arglist>(Array&lt; T &gt;, operator-=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1DEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>6d4929e6bc83f10141534fdefec467c0</anchor>
      <arglist>(Array&lt; T &gt;, operator *=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1DEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>0acc0d156b75206b88366608a6b39626</anchor>
      <arglist>(Array&lt; T &gt;, operator/=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1DEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>867685c86f7f1dcaf2c054f17a8d0424</anchor>
      <arglist>(Array&lt; T &gt;, operator%=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS2DEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>bc656dc5ea9e5b4ea9d0c54dcfb6af85</anchor>
      <arglist>(class TT, Array&lt; TT &gt;, operator+)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS2DEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>5c044067f2211b44fc720891a07b4224</anchor>
      <arglist>(class TT, Array&lt; TT &gt;, operator-)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS2DEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>794f4e30951bf63b8f7e67bacc5dec90</anchor>
      <arglist>(class TT, Array&lt; TT &gt;, operator *)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS2DEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>0b3482db43fc10493f0a9384187ac0d3</anchor>
      <arglist>(class TT, Array&lt; TT &gt;, operator/)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS2DEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>5409911d62833d706e450ea998105cd4</anchor>
      <arglist>(class TT, Array&lt; TT &gt;, operator%)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt;</type>
      <name>operator-</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>1fdab3ece3ce2a0af7b45f6db5cb3aef</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>sin</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>27f419582c60c22f5b8b97de1d861756</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>cos</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>54fd2a17b70c71fd852bc308a27a99d7</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>tan</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>ee45c1656f459986f684990ac987257e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>asin</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>1b3962c58318ee1e7c5a0600632f2419</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>acos</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>c5aec9c3ad4d810da8ce7de33dfe22de</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>atan</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>b48182d9fd5363e3870af3638c2b1e45</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>sinh</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>5e1a1e512bf9f4cdcdfad1f2cb70b83d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>cosh</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>307e6eb5ac4714852d9a3227d5bf7c83</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>tanh</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>f36c4383d2c41ef826243017c5ebc112</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>asinh</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>afad059b32f296beca5f72d373cfc1ba</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>acosh</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>fb8b72548df55b2949ac58b1caffd4ee</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>atanh</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>f1dd70fbaa0008a38e2f7df215c8538c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>exp</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>31b988de8380cbb76ef43bdcf6e9df64</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>log</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>3342a813ef9b387ef7b4a722320a8fd3</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>log10</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>b5e1ea7961b52e30f225aa73545a9943</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>erf</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>e84ef39d158bf1da1fcaf1762d258d38</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>erfc</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>ebda60b6001a13a5641a9d1175b3ebdc</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>sqrt</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>694a92b6cf45fa7222f153655081ccce</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>cbrt</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>991593fc2b0f91e9dba91e269a86e2e1</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>square</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>fa5ec54be267c94eb9abe13b04ca485f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>cube</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>e4c84de524757489e8c46b62d13b4929</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINERFUNC1DEC</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>e8429d61346a8e5186dc65d4ae0d4a91</anchor>
      <arglist>(Array&lt; T &gt;, pow)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>exp</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>bee17b8e1a4dbb0ce7ead635785960e4</anchor>
      <arglist>(double base)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>ceil</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>8aabec394effee8e1c4dd8ef73bab27d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>floor</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>a548a90f0e277dd46525a855e0ac2e77</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>abs</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>c38d131b75cbdc9ba154eef49076d651</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>decibel</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>3255858d43158a5cf549fb53a9850a78</anchor>
      <arglist>(double level=0.0)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>linear</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>4b86fea4644dd329c18afeb8a45fe193</anchor>
      <arglist>(double level)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>min</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>2daae32586b558e67780d0f209a3bfaf</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>minIndex</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>1e41c8154d094cdfbf537ec3becf9c50</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>minIndex</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>d0f7c8f91f882f57c471d72119b7f974</anchor>
      <arglist>(double &amp;min, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>max</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>9f41053235339ee57673628a8caa71c6</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>maxIndex</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>e7890347f84939d92ad98a04eca4a43f</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>maxIndex</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>fcdc1480c82991a1c5b9f67363a7966d</anchor>
      <arglist>(double &amp;max, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMax</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>a537891d403039139cec37bf460ea692</anchor>
      <arglist>(double &amp;min, double &amp;max, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxIndex</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>945d00f9c73199c77f4242dbb2a11d95</anchor>
      <arglist>(int &amp;minindex, int &amp;maxindex, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxIndex</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>9b94cb4d7e34ee84f15d85885331ccd6</anchor>
      <arglist>(double &amp;min, int &amp;minindex, double &amp;max, int &amp;maxindex, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>mean</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>390a828aea5c9aced384616b9db9b9fa</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>mean</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>3d6f76d833cbdf8ada0dadc83f1b8954</anchor>
      <arglist>(double &amp;stdev, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>8bcce45cc62f3b70a4ab803c303d18af</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>bbc8192d6be977543f780e62d3e0a1ec</anchor>
      <arglist>(double mean, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>varianceFixed</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>7eff5be3845501a15fdf28af92170875</anchor>
      <arglist>(double fixedmean, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdev</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>967a09b7d2321c67772e7295d5ced025</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdev</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>2ca00f221f597d909b823fce172ba66c</anchor>
      <arglist>(double mean, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdevFixed</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>ec898ac645d9216840366bf7feabb743</anchor>
      <arglist>(double fixedmean, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sem</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>698b412ece9b242c157ce33522d0fec0</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sem</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>75ec0dd0a8fce766d1a891af586c0649</anchor>
      <arglist>(double mean, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>semFixed</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>cf5b1d8574f9fa395d329e4fb2fdef9f</anchor>
      <arglist>(double fixedmean, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>absdev</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>91c1a5e0c494b9b95487259bc398c365</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>absdev</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>89d0bff01a5fbb8d857183dc33cb7f47</anchor>
      <arglist>(double mean, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rms</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>038ca442dc81daa0fb07879dbe83dfba</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>skewness</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>6a3ca34eed638adb8c3d2dbaa69c37e6</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>kurtosis</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>9868a23986e9a77f6fc8434a1cf2acd6</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sum</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>60ffb5f734ccf9ea9af042611c96a015</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>squaredSum</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>c87152fd5d94e949919a4d8500913b6e</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>power</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>65f793b180904a95c9936b7e9ae1f398</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rank</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>de74de22f174d4f085849e81474c28e0</anchor>
      <arglist>(int first=0, int last=-1)</arglist>
    </member>
    <member kind="function">
      <type>ostream &amp;</type>
      <name>save</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>118e238b055c05704c61f759da1d3e13</anchor>
      <arglist>(ostream &amp;str, int width=8, int prec=3) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>save</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>7820d2ecddcfe79b04ed845b01a237a2</anchor>
      <arglist>(const string &amp;file, int width=8, int prec=3) const</arglist>
    </member>
    <member kind="function">
      <type>istream &amp;</type>
      <name>load</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>1c3a4523948e9eda84af88e47b172b1a</anchor>
      <arglist>(istream &amp;str, const string &amp;stop=&quot;&quot;, string *line=0)</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>load</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>b5cc3410ef29739f2224f43edc4513f7</anchor>
      <arglist>(const string &amp;file, const string &amp;stop=&quot;&quot;)</arglist>
    </member>
    <member kind="friend">
      <type>friend bool</type>
      <name>operator==</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>b6db939776ff5028cc03399380242d00</anchor>
      <arglist>(const Array&lt; TT &gt; &amp;a, const Array&lt; TT &gt; &amp;b)</arglist>
    </member>
    <member kind="friend">
      <type>friend bool</type>
      <name>operator&lt;</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>a6269bb80b80243bab725ffca4bf2730</anchor>
      <arglist>(const Array&lt; TT &gt; &amp;a, const Array&lt; TT &gt; &amp;b)</arglist>
    </member>
    <member kind="friend">
      <type>friend Array&lt; TT &gt;</type>
      <name>convolve</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>222e65a46707d9fc2d06962f45d2ff4f</anchor>
      <arglist>(const Array&lt; TT &gt; &amp;x, const SS &amp;y, int offs=0)</arglist>
    </member>
    <member kind="friend">
      <type>friend ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>ae40ac39ceb6499de90ad6689f638a47</anchor>
      <arglist>(ostream &amp;str, const Array&lt; TT &gt; &amp;a)</arglist>
    </member>
    <member kind="friend">
      <type>friend istream &amp;</type>
      <name>operator&gt;&gt;</name>
      <anchorfile>classArray.html</anchorfile>
      <anchor>90f4ec91af822cc8079870962852cd39</anchor>
      <arglist>(istream &amp;str, Array&lt; TT &gt; &amp;a)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>BasisFunction</name>
    <filename>classBasisFunction.html</filename>
    <member kind="function">
      <type></type>
      <name>BasisFunction</name>
      <anchorfile>classBasisFunction.html</anchorfile>
      <anchor>cfd675fd24a5771fb4b71ea329e58d67</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~BasisFunction</name>
      <anchorfile>classBasisFunction.html</anchorfile>
      <anchor>7c2746b937e3f013a9208e1aeb703a36</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classBasisFunction.html</anchorfile>
      <anchor>2726a9075bffc70f42ce0be9c36eadbc</anchor>
      <arglist>(double x, ArrayD &amp;y) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>basis</name>
      <anchorfile>classBasisFunction.html</anchorfile>
      <anchor>0b6457d0730d4c39b42206b7f22327ed</anchor>
      <arglist>(double x, ArrayD &amp;y) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classBasisFunction.html</anchorfile>
      <anchor>98f8cf8d6686f08b92aeaf17a46b2ca1</anchor>
      <arglist>(const ArrayD &amp;c, double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>value</name>
      <anchorfile>classBasisFunction.html</anchorfile>
      <anchor>4337ffed75b5c4e528a28b2d2cbacbe3</anchor>
      <arglist>(const ArrayD &amp;c, double x) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>CyclicArray</name>
    <filename>classCyclicArray.html</filename>
    <templarg>T</templarg>
    <member kind="typedef">
      <type>T</type>
      <name>value_type</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>83773a5f80d4b7ad5f03b22588b55124</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>T *</type>
      <name>pointer</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>3342bfe1304d7fd1e31fbbe72f3a1230</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>T &amp;</type>
      <name>reference</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>b6bdccce121ea339ca2768152e480dc2</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const T &amp;</type>
      <name>const_reference</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>df02493a61ada0a857af29aee44964cb</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>int</type>
      <name>size_type</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>ea18415364ab53c8c8d00fecf37b12e1</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CyclicArray</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>19e3162c6845b5fa9ec9cfaa40ad58e1</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CyclicArray</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>f61ff6c10f4582cded8ad0c8669cf92c</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CyclicArray</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>6d93b0259955774ab1813aff99609012</anchor>
      <arglist>(const CyclicArray&lt; T &gt; &amp;ca)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~CyclicArray</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>f124caa2f6043bf3e381e2e93dd666be</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const CyclicArray&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>28d4ac866427e96991a801d05834a581</anchor>
      <arglist>(const CyclicArray&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const CyclicArray&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>f1e74491dfe4388313f87bfe334a509d</anchor>
      <arglist>(const CyclicArray&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>size</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>37f9dfb0cfdb74ecc724d8bfdc650997</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>accessibleSize</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>4cc39fa3475bc06673edef1ca45eb56a</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>minIndex</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>55f4afcd5bafec15f10f0abef0a59a1a</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>empty</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>3c392d94f9070898c645a5031352a0da</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>resize</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>18f99835093e7a900c7a3e5cf0dfe97a</anchor>
      <arglist>(int n, const T &amp;val=0)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>clear</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>e12d5551d6813971d4a859da3ab070fa</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>capacity</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>7b390b35ace01259f7efb071d2cd9f41</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>reserve</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>d3830a58d3b932b855fc2e7591e7ef32</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>operator[]</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>202e2d2a69018fe5880562e0ad93fbe0</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>operator[]</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>c42a031e01cfc7f7b623be86e80ccd45</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>at</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>86cf418d260f73e03813a14f5fcf05f2</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>at</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>009effffb0c43003158f8f1a58ade66a</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>front</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>7051b61111018f066f33e33f7ec3e8f3</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>front</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>b533bf231c6111f50d7a8a60817eaad9</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>back</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>04f545c7b3551ec472017a867cf88d2c</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>back</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>29fdb6f6e2c8633b6231fc91e24ae4fc</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>7b2af500c02f76743b009e02b72b962a</anchor>
      <arglist>(const T &amp;val)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>pop</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>a1af2667cdd0bd8ed2f0fb40aef83725</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>maxPush</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>d0e230627fb8edc6a2601c7595d9bec4</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>T *</type>
      <name>pushBuffer</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>f10f3838a971157a4a77a235e6095da4</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>a12a4b073645c02b2c88492c32bde924</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>readSize</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>3639377405e2a941bc3551cece366de8</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>readIndex</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>9992cd136797f6fe9b28b2337caa45ff</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>read</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>4960126f9fa8759aaa526a8de07c8f62</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>min</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>54c75e05a70e607c8626ae5803db301a</anchor>
      <arglist>(int from, int upto) const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>max</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>585b085969123239c5abd374b4183df9</anchor>
      <arglist>(int from, int upto) const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>minAbs</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>95bbe8d7dd9dec587ebea374383bcd4e</anchor>
      <arglist>(int from, int upto) const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>maxAbs</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>81169ead284c715bfef9317b07b8fb17</anchor>
      <arglist>(int from, int upto) const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>mean</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>bb7247757dd40181c2eb793b76a700a0</anchor>
      <arglist>(int from, int upto) const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>variance</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>fb8a91b9ae514bae1095aed40e7fd0fd</anchor>
      <arglist>(int from, int upto) const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>stdev</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>7d63324f433870c7309522ea4d0c22ce</anchor>
      <arglist>(int from, int upto) const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>rms</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>39f3f6251395671d0fb573d4adc1514a</anchor>
      <arglist>(int from, int upto) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hist</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>d304fdebba400548fc5e3f07150ecc0a</anchor>
      <arglist>(SampleData&lt; S &gt; &amp;h) const</arglist>
    </member>
    <member kind="friend">
      <type>friend ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classCyclicArray.html</anchorfile>
      <anchor>2f9b1e08a1088f3085bd21bc1bec94b2</anchor>
      <arglist>(ostream &amp;str, const CyclicArray&lt; TT &gt; &amp;ca)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>CyclicSampleData</name>
    <filename>classCyclicSampleData.html</filename>
    <templarg>T</templarg>
    <base>CyclicArray</base>
    <member kind="typedef">
      <type>double</type>
      <name>range_type</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>d07965e761c538da23023e95d35719e4</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>LinearRangeIterator</type>
      <name>const_range_iterator</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>c3d4582ed34c82e6019018389019e6b0</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CyclicSampleData</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>61d796eac76e305416e090f4a192607d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CyclicSampleData</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>88ea998305c7d799e45964b88bb1e218</anchor>
      <arglist>(int n, double stepsize=1.0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CyclicSampleData</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>88ada74c1c5df9b124b8b1b2c1d1ced2</anchor>
      <arglist>(int n, double offset, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CyclicSampleData</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>53879b5f94a5995d73755a2d460a1e4a</anchor>
      <arglist>(double duration, double stepsize=1.0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CyclicSampleData</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>b05ec481eafb6e8286f1350cc567d4c4</anchor>
      <arglist>(double duration, double offset, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CyclicSampleData</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>50bfe858d270e5c0513bb112e833ca3c</anchor>
      <arglist>(const CyclicSampleData&lt; T &gt; &amp;csd)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~CyclicSampleData</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>a61611aa133f604d36549ea7339cd8d3</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>minIndex</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>55f2f78480f733459b606ca3786be7e4</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>minPos</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>8e69a862f298c916f8e94ca311b5c96c</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>resize</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>a7bcf924410e74accb89dace8615470c</anchor>
      <arglist>(int n, const T &amp;val=0)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>resize</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>3ee78ed299ebce3efb8af5ac5932555f</anchor>
      <arglist>(double duration, const T &amp;val=0)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>reserve</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>f01b9b2bb4576ce4bad89641470d972a</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>reserve</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>cf3225c33b37ef83e1a076c50e5a6592</anchor>
      <arglist>(double duration)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>offset</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>254dbf8007af38ee649f7f3a0d930575</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOffset</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>64e76b79be55773530ded40c54fb9e6c</anchor>
      <arglist>(double offset)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stepsize</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>eb932da82ec1a75428bbecfdbee898fe</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setStepsize</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>40ba1493448eb01ac7429b8ecf03f9cb</anchor>
      <arglist>(double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRange</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>c047fe8aea083fba0c936956e7225cce</anchor>
      <arglist>(const T &amp;offset, const T &amp;stepsize)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>length</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>ca3b82185394063edac3451542b2530b</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rangeFront</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>fb685706b9653193365a03ef555d577e</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rangeBack</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>3c7d6a272896262c529c2213894382c1</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>shift</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>798f4e12896d3dd135b1c178e8110902</anchor>
      <arglist>(double val)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>2cc4d99f5bd3ba1b5e298d98ca9cbeae</anchor>
      <arglist>(double scale)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>pos</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>a4ae8d7b87953a529870be46bd020609</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>interval</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>50e31654818e7254afa7cae078dd325f</anchor>
      <arglist>(int indices) const </arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>index</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>2c04112f1e30e4fe37442a0d36b2ed66</anchor>
      <arglist>(double pos) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>indices</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>dfe7714ba137f7d639fd3ca50f2e3b14</anchor>
      <arglist>(double iv) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>contains</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>dd87f52276e74161675300b361e4d422</anchor>
      <arglist>(double pos) const</arglist>
    </member>
    <member kind="function">
      <type>const CyclicArray&lt; T &gt; &amp;</type>
      <name>array</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>dcb4b6fc44c4dfeb73b77e4172623eb5</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>CyclicArray&lt; T &gt; &amp;</type>
      <name>array</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>5d1e1d321a3bf7c11b21dbd4df9b343f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>LinearRange</type>
      <name>range</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>115191c62d3cf867250232d60ae615b6</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>max</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>fd266e1c61af151cab05eab1cc7a5d71</anchor>
      <arglist>(long from, long upto) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>max</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>41c8e5728dfadcd8cc72489f6399614a</anchor>
      <arglist>(double time, double duration) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>max</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>75916675cf98baa6f7f94c5998b33c19</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>min</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>a8a462621f4f23956fb8e3e0259bce3c</anchor>
      <arglist>(long from, long upto) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>min</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>937aaf44b72db9196c685d3961515874</anchor>
      <arglist>(double time, double duration) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>min</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>22e2272fc0935e8ba1b539dbd42cceb0</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>maxAbs</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>278492531e07a8f35ff1df587c4b352b</anchor>
      <arglist>(long from, long upto) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>maxAbs</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>3b8ad96db161d0d520977b8c4a046149</anchor>
      <arglist>(double time, double duration) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>maxAbs</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>efc2b2c431be99b976268eb11ecb002b</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>minAbs</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>1e531d2bf575d5a1fd44f6d92747df1c</anchor>
      <arglist>(long from, long upto) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>minAbs</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>f27e6ac9b71097f253be8968b2bd6337</anchor>
      <arglist>(double time, double duration) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>minAbs</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>7f3035434c72a7b5543bbaae51bb21c8</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>mean</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>5962741d8088a3b5ea9ef2fd6d6deeb8</anchor>
      <arglist>(long from, long upto) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>mean</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>14a3e72c1c7b158b1c8da0349be22cbb</anchor>
      <arglist>(double time, double duration) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>mean</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>1682a053e39a0c44655048a72a23ca77</anchor>
      <arglist>(double time, SampleDataD &amp;md, double width=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>fca47a4dbee8b0541dd8ccb4a467af01</anchor>
      <arglist>(long from, long upto) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>559407b5659ac9c15a23b5a81c2036e7</anchor>
      <arglist>(double time, double duration) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>variance</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>458dfed91ac82c6989e33d5c4a4fb9a9</anchor>
      <arglist>(double time, SampleDataD &amp;vd, double width=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdev</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>8d2c71f03352361acd08a1a076d03005</anchor>
      <arglist>(long from, long upto) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdev</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>6530532cc03ab595f268517834ac96a8</anchor>
      <arglist>(double time, double duration) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>stdev</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>006ddd7e1780a717c0318275c47d74ed</anchor>
      <arglist>(double time, SampleDataD &amp;rd, double width=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rms</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>5a93d35494e3d59ebca873f6ac09848d</anchor>
      <arglist>(long from, long upto) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rms</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>2d5cbf7be6014c60f92b593aff84c04b</anchor>
      <arglist>(double time, double duration) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rms</name>
      <anchorfile>classCyclicSampleData.html</anchorfile>
      <anchor>e3d815db79ea44186728d2238a3bc5c8</anchor>
      <arglist>(double time, SampleDataD &amp;rd, double width=0.0) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Detector</name>
    <filename>classDetector.html</filename>
    <templarg>DataIter</templarg>
    <templarg>TimeIter</templarg>
    <member kind="function">
      <type></type>
      <name>Detector</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>6ed4d83607c5315a29486553185f3a78</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Detector</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>f2e35ff10c1f26bd49b42f260055375d</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~Detector</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>63a14e80a0ea57b9bae02e8b59c5041c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>init</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>255d5b82af2da29c8b7404c1c8a4694c</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, const TimeIter &amp;firsttime)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>peakTrough</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>098c4eb0be39b0435d95a32524cd745d</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventList &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>peakTroughHist</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>a74b453b7086479d8566bda1dd1adea0</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventList &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>peak</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>2a79017d81fd86b5c2f25a9ac694d7bc</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>peakHist</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>1b98a2653d3a307c03ed11d7f471d2f8</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>trough</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>dfe9321aa41550f3a633b8b4d1153979</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>troughHist</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>79dd3fb207338a0a47af8f2a4e030a6e</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dynamicPeakTrough</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>7499a32c12fb31347b57f275cad22bec</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventList &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, double delay, double decay, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dynamicPeakTroughHist</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>576b69a4ab8ac8e7523cd2a12e70757a</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventList &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, double delay, double decay, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dynamicPeak</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>756cb90f586127e5c61beca9478a719c</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, double delay, double decay, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dynamicPeakHist</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>b2a77fd3fce8991877744b7f4621009d</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, double delay, double decay, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dynamicTrough</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>ab5525357e050d95e1e41cf8e5576d29</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, double delay, double decay, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dynamicTroughHist</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>9bec2a4945481b3eb53ce8e6bc2904f3</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, double delay, double decay, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rising</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>9b63b3edd84999af51fdd733de4e6a4e</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>falling</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>2acb4d4c71c0b4dcad2f66470ba3f0ac</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dynamicRising</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>9091b7c865c3a14da008c91e66cb33a5</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, double delay, double decay, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>dynamicFalling</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>8174db3ef46078062f303a8e74c89df6</anchor>
      <arglist>(const DataIter &amp;first, const DataIter &amp;last, EventData &amp;outevents, double &amp;threshold, double minthresh, double maxthresh, double delay, double decay, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>goodEvents</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>5f6f017e486519de3a5f5b9813ea6c7d</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>badEvents</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>305a5755f016392c382c3b68ec64ca4a</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setHistorySize</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>74be5fefe140155861bf97a016889e1d</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>historySize</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>057e35210070b1a97eb390ab4c54b889</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearHistory</name>
      <anchorfile>classDetector.html</anchorfile>
      <anchor>dc9517144506e2a55e8466fcdac58719</anchor>
      <arglist>(void)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>EpanechnikovKernel</name>
    <filename>classEpanechnikovKernel.html</filename>
    <base>Kernel</base>
    <member kind="function">
      <type></type>
      <name>EpanechnikovKernel</name>
      <anchorfile>classEpanechnikovKernel.html</anchorfile>
      <anchor>ea46e0ca927e9d940a9d3ba98fd87884</anchor>
      <arglist>(double stdev)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classEpanechnikovKernel.html</anchorfile>
      <anchor>bb3ee3b190b52a4de770c4cd6b4034a9</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>value</name>
      <anchorfile>classEpanechnikovKernel.html</anchorfile>
      <anchor>14f36693977d7c26b8260efa8a9187ae</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setScale</name>
      <anchorfile>classEpanechnikovKernel.html</anchorfile>
      <anchor>c35442034b631c4c1f3b6944a7a9e594</anchor>
      <arglist>(double scale)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>mean</name>
      <anchorfile>classEpanechnikovKernel.html</anchorfile>
      <anchor>6cf4a6d75a0d029bb70fc714b6186303</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>stdev</name>
      <anchorfile>classEpanechnikovKernel.html</anchorfile>
      <anchor>125fdf16d18d0685c251f20fd77cc704</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setStdev</name>
      <anchorfile>classEpanechnikovKernel.html</anchorfile>
      <anchor>26d2905e5d10650b6a68c23ade449dfa</anchor>
      <arglist>(double stdev)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>max</name>
      <anchorfile>classEpanechnikovKernel.html</anchorfile>
      <anchor>4cad18f783126997574a726300ae7e63</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>left</name>
      <anchorfile>classEpanechnikovKernel.html</anchorfile>
      <anchor>56c5ddbcf960c877f6b6b48da01919ca</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>right</name>
      <anchorfile>classEpanechnikovKernel.html</anchorfile>
      <anchor>dee9bb2ba70441d89eb10acc6ef24356</anchor>
      <arglist>(void) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>EventData</name>
    <filename>classEventData.html</filename>
    <member kind="typedef">
      <type>EventIterator</type>
      <name>const_iterator</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>570a215496b2cf0dc5622f3595e16031</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventData</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>5e1c4c4297134fdddc3607347558c185</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventData</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c97fcf993ce6e1b7b8258b548d07ec54</anchor>
      <arglist>(int n, bool sizebuffer=false, bool widthbuffer=false)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventData</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>7f269897a0e62d131ad7fb730eb0e1dd</anchor>
      <arglist>(int n, double tbegin, double tend, double stepsize=0.0001, bool sizebuffer=false, bool widthbuffer=false)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventData</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c52de52b44e2842c35493499dbe151a9</anchor>
      <arglist>(const EventData &amp;events)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventData</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>1ded2032df77fb17ecd592c884379e7b</anchor>
      <arglist>(const EventData &amp;events, double tbegin, double tend)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventData</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>bb60a57ea47dcf58bb1dbe6ce58ab0fc</anchor>
      <arglist>(const EventData &amp;events, double tbegin, double tend, double tref)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventData</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>7678f0842b4118a7416110254a11e3da</anchor>
      <arglist>(const ArrayD &amp;times, double tbegin=-HUGE_VAL, double tend=HUGE_VAL, double stepsize=0.0001)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~EventData</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>066ae7fde154575fd5fe756151478681</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>size</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>007c932d5dd89a3721928bf3d67986ed</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>empty</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>097ca62e07272972ea11613e852ae8ef</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>b6bdcd91797af4316285e1b41d4a9a0a</anchor>
      <arglist>(long nevents, double dflt=0.0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>f79695e60b2802d472866f1378b5d538</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>capacity</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>14a9d976cc5f9aa9f0920a53610ec415</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reserve</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>34a2076829bd6ef869d6400b1657f25a</anchor>
      <arglist>(long n, double dflt=0.0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>free</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>d3fcb788908bb7b60e092ae24b9ea5f2</anchor>
      <arglist>(long n=0, double dflt=0.0)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>cyclic</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>3a5421d86c3f605370168abcb349db9d</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setCyclic</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>5c9ef3eb8f8cd26415901159e34a42de</anchor>
      <arglist>(bool cyclic=true)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>sizeBuffer</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>9ccb885dd8da8da63368543ef92eccc2</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>useSizeBuffer</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>6ae1f61e912aff4fba7bcd2202f93a09</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSizeBuffer</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>45b1abd2521422419dbbb115d5be90ce</anchor>
      <arglist>(bool use=true)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>widthBuffer</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>b2c231722f2d6ec645481ff0b5b487a9</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>useWidthBuffer</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>4e975bf6dec19b93149bc3dcc03dc031</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setWidthBuffer</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c75e9253a42113cf7d0117f0c9c9c31c</anchor>
      <arglist>(bool use=true)</arglist>
    </member>
    <member kind="function">
      <type>LinearRange &amp;</type>
      <name>range</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>d5ca1391dcb88d8e002804a104919542</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>range</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>8662da7dd577bf2ca9159a5341994776</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>offset</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c6fb26553c9d9c452c88b847804a00ba</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOffset</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>a6da4d7bc630fc97e19a92ed40453d69</anchor>
      <arglist>(double offset)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>length</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>e03ca4c29192ee51daf19905b4da2817</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setLength</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>a0f887f6e5f5071690ac7c9158b4dd5c</anchor>
      <arglist>(double duration)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stepsize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>934eb4e547fd064a8b3fd45ec8f1bc7b</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setStepsize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>b5d6587cd1f458b38707024f4a3198bb</anchor>
      <arglist>(double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rangeFront</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>a01e1da9aafb6ed913c2b21bd7d3ee49</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRangeFront</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>037357a64a5eee6685f2afd1ea614cda</anchor>
      <arglist>(double front)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rangeBack</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>410cec40f7d90c5895d41fbc55af331c</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRangeBack</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>fbde1a12718bf68ceb801c6af0e49b5d</anchor>
      <arglist>(double back)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>signalTime</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>ba0f65352e1eba4250973304394433d2</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSignalTime</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>ba1703e7265bf4fcfd99614da79dfe09</anchor>
      <arglist>(double s)</arglist>
    </member>
    <member kind="function">
      <type>EventData &amp;</type>
      <name>operator=</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>7bb837b84e7a8475d96141af19c2f41e</anchor>
      <arglist>(const EventData &amp;events)</arglist>
    </member>
    <member kind="function">
      <type>EventData &amp;</type>
      <name>operator=</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>ea4f9fdaf7c0fd8a64dfff7ed0fbc60b</anchor>
      <arglist>(const ArrayD &amp;times)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>assign</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>76658a004aff70f3a5daacec9c087e64</anchor>
      <arglist>(const EventData &amp;events)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>assign</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c96a88e76c07ae8ac50aa5f11c7850af</anchor>
      <arglist>(const EventData &amp;events, double tbegin, double tend)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>assign</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>7a03c2ead0778423dce7e625dae4184d</anchor>
      <arglist>(const EventData &amp;events, double tbegin, double tend, double tref)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>assign</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>a6023d77714f5c7d4d55a5acfa717510</anchor>
      <arglist>(const ArrayD &amp;times, double tbegin=-HUGE_VAL, double tend=HUGE_VAL, double stepsize=0.0001)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>016a4dabbc9564f7406140f2ffcf2cc4</anchor>
      <arglist>(double tbegin, double tend, double *events, long &amp;nevents) const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>a5add3ec2cac222d2980e010f1fc63fa</anchor>
      <arglist>(double tbegin, double tend, double tref, double *events, long &amp;nevents) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>9398c48ee82abe814552caee3ce8880f</anchor>
      <arglist>(double tbegin, double tend, vector&lt; double &gt; &amp;events) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>e13635de85e69a03509039b3eac13572</anchor>
      <arglist>(double tbegin, double tend, double tref, vector&lt; double &gt; &amp;events) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>efd71a92987170bce902e0fe15b505e4</anchor>
      <arglist>(double tbegin, double tend, ArrayD &amp;events) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>f0eeb0d3a743b8a150c88d35634024a0</anchor>
      <arglist>(double tbegin, double tend, double tref, ArrayD &amp;events) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>0a33124107e064817ece639412e3de12</anchor>
      <arglist>(double tbegin, double tend, EventData &amp;events) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>copy</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>735265e6b5481dd9cd732c92067f37cb</anchor>
      <arglist>(double tbegin, double tend, double tref, EventData &amp;events) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator[]</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>04596d85d9d33369d914949a9d5a8eab</anchor>
      <arglist>(long i) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>operator[]</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>fd58abe0aa4db4e73dfa6223a163e349</anchor>
      <arglist>(long i)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>at</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>0887c592a4a4b09a067d3c156e115f45</anchor>
      <arglist>(long i) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>at</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c507e00d118255210f77dd122e9aefaf</anchor>
      <arglist>(long i)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>front</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>4403e12204a5336b3eced4921f2cf0dc</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>front</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>678f19bf0f3f476cfcdbf78fe156a10a</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>front</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>2c9a076a4dda3fbc235dbf9d4be1a429</anchor>
      <arglist>(long n) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>front</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>afbf879ed9cfb8b83e0b575d54571ee0</anchor>
      <arglist>(long n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>back</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>56ac863cba80210bdd099eb8fd613bdd</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>back</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>e5742f44b48d0f0a4f43897fa064da62</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>back</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>e1d01d110b68d7b2220709f55ad36ed6</anchor>
      <arglist>(long n) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>back</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>e3750906c034fd22ce71c3feb0454956</anchor>
      <arglist>(long n)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>begin</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>ad35883807d8c2a8b06c2810b4b12c3b</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>begin</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>b903e9fe0543790166f2ef55e86f41c6</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>end</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>bad680fc10931a0afa970123a0010b68</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>eventSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>105bd1f316cc46f1755720e90d386ca6</anchor>
      <arglist>(long i) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>eventSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>abfbef27c8e22b8401847047c010707a</anchor>
      <arglist>(long i)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frontSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>d2f786a1e3c2ba7ecbd9fc5493cec98a</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>frontSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>138744e4677e4b74e7d7f5deeeb1a672</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frontSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>af37a7b6d41995f2e85f0fdc4c2181b7</anchor>
      <arglist>(long n) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>frontSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>74a3d36258601cbbf500748cf39c97da</anchor>
      <arglist>(long n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>backSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>35948179533b43ce524e2f932a49cbec</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>backSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>83c22dc4de86f619138bb8985fac4a13</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>backSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>2025a25f01a2dc7f8e6af1eb5701b1ab</anchor>
      <arglist>(long n) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>backSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>f2eb0fae260574e19c84f84e6c89fc84</anchor>
      <arglist>(long n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>eventWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>5e596bd1c7825ff95d989062d47ac33e</anchor>
      <arglist>(long i) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>eventWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>7717a78d4a0112cddce83887fa7b1f82</anchor>
      <arglist>(long i)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frontWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>f5f991ea11cf9e362219f867ad806ed7</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>frontWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>63e7cd48de6e2dfb799a6eb438fab922</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frontWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>f01168c604070c22aaa686bcca036e81</anchor>
      <arglist>(long n) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>frontWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c905e2544a13d28662637dd137d331b9</anchor>
      <arglist>(long n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>backWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>5f083e2415951dcd297b1c0669e71a6a</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>backWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>40395eb2e595d1d9316e772f1aa41c07</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>backWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c7bd06f93f1a8c4f9decccf9babd9a05</anchor>
      <arglist>(long n) const</arglist>
    </member>
    <member kind="function">
      <type>double &amp;</type>
      <name>backWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>2a0b4a280a14981b28114cbbe0d1c22a</anchor>
      <arglist>(long n)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>6354c1e8b52a048c660728efeeecbe5f</anchor>
      <arglist>(double time, double size=1.0, double width=0.0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>cd79239668da6ff77e57176122a5dd0e</anchor>
      <arglist>(const ArrayD &amp;time, double size=1.0, double width=0.0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>insert</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>8bc89694ce0a87b9c0f772b8128b8d72</anchor>
      <arglist>(double time, double size=0.0, double width=0.0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>insert</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>87af5b4c9ab9ce51ea2d6d778c98d11c</anchor>
      <arglist>(const EventData &amp;e)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>erase</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>65e3b58ca65599f364ff4c2548cd85d7</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>erase</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>21fdffc5ed44a9576a9a65c531e81f23</anchor>
      <arglist>(const_iterator iter)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pop</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>1e92f8b079ba5b89940c703ccd8d2ac5</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sum</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>3e2c33bf50a926c7fa11f2f34fa26329</anchor>
      <arglist>(const EventData &amp;e, EventData &amp;all) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sync</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>11bcce5c93d52ae53cf11290f0a50f2e</anchor>
      <arglist>(const EventData &amp;e, EventData &amp;s, double bin) const</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>operator+=</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>f2bc1575f1f4af41cfd75570786c620f</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>operator-=</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>ff4490c71ad156d8dc342de9250afdb6</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>operator *=</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>6e3c5d4bc50201a16be0adfb95589987</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>operator/=</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>820e410a374a68f3cfc366ebf2cfa7a6</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>operator+=</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>398197e6b76c9c526dd78bf3a84522d5</anchor>
      <arglist>(const EventData &amp;e)</arglist>
    </member>
    <member kind="function">
      <type>EventData</type>
      <name>operator+</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>8d5ee61b91e8e53432a76d214d2892c0</anchor>
      <arglist>(const EventData &amp;e) const</arglist>
    </member>
    <member kind="function">
      <type>EventData</type>
      <name>operator *</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>52b849a19d8c5065b10c2a220c56025b</anchor>
      <arglist>(const EventData &amp;e) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>check</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>5d84a38c9d39b0ef000af466aa50f0de</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>string</type>
      <name>message</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>d0eb3138d17e3c1559a3ad944808c0bd</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>currentEvent</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>96d3a22080498b80a35ea2dffa508e06</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>minEvent</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>142137bb15717b20be1fb8dfa8cf6765</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>minTime</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>20f2f8c22ae68ad891f61d5a8ef77b1c</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>mode</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>42ab2357fbfa110df08c0b65d8b8c577</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMode</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>0de33d69a6b021ce050b90342ead228f</anchor>
      <arglist>(int mode)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>source</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>439f5b2e65993ffacb36c88459d382ab</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSource</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>a6b54e8807dc1425c56e0f04d1966f14</anchor>
      <arglist>(int source)</arglist>
    </member>
    <member kind="function">
      <type>const string &amp;</type>
      <name>ident</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>91253422ac2b7deee94ebbacb7352580</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setIdent</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>982d9c7f8d04d9b79233cc0458ae7cfe</anchor>
      <arglist>(const string &amp;ident)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>minSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>bbe420abb52e7ffd55936954f8b28d36</anchor>
      <arglist>(double tbegin, double tend) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>maxSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>8258a8f5e12237c8c766a08ce38fac15</anchor>
      <arglist>(double tbegin, double tend) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>bce3c6ac678a5d945abf39b9e17add06</anchor>
      <arglist>(double tbegin, double tend, double &amp;min, double &amp;max) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>meanSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>3a1787958081979b1d4a52583a723c8b</anchor>
      <arglist>(double tbegin, double tend, double &amp;stdev) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>meanSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>2d1c209fc0298fff04afe603dde13e7c</anchor>
      <arglist>(double tbegin, double tend) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>meanSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>f5d1f00a34be1165d6ac1bc41f63001b</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>meanSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>659b8188141a4887e5de6d003b05f29c</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMeanSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>cc805d5ad7e77cafb2afe97fcc33720b</anchor>
      <arglist>(double meansize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateMeanSize</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>780f855be04c2310dc5ae0ad741f305a</anchor>
      <arglist>(int n=1, double size=0.0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sizeHist</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>091359fa94fdaca78b85f5f79bbe9564</anchor>
      <arglist>(double tbegin, double tend, SampleDataD &amp;hist) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>minWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>d539d4f51a729e2e68353e0ac028f567</anchor>
      <arglist>(double tbegin, double tend) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>maxWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>d46252d39fdac3d03d1a4a76d392f2a1</anchor>
      <arglist>(double tbegin, double tend) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>d23e7be1033c2ee602819e71eeeafab9</anchor>
      <arglist>(double tbegin, double tend, double &amp;min, double &amp;max) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>meanWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c5844ca69cfdbd820321bec3ac3e0ee0</anchor>
      <arglist>(double tbegin, double tend) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>meanWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>61ba4e435140bbb84bad05a162b8531f</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMeanWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>e0990145313e0b41291ed30209de714c</anchor>
      <arglist>(double meanwidth)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateMeanWidth</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>5300ab6d1946ca472dc3b04fc645d791</anchor>
      <arglist>(int n=1, double width=0.0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>widthHist</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>db8c8c466eb10181a570be5dc8f00995</anchor>
      <arglist>(double tbegin, double tend, SampleDataD &amp;hist) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>meanRate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>2f6d7f9fccbbd838b0af4a285fb91f3a</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMeanRate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>fe6ee62f28e0171f99829d6c8d4efc9a</anchor>
      <arglist>(double meanrate)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateMeanRate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>8581600d51b7bb7104cb91d185300875</anchor>
      <arglist>(int n=1, double rate=0.0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateMean</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>0f642a60e99fcbdceeb294308892daf7</anchor>
      <arglist>(int n=1, double size=0.0, double width=0.0, double rate=0.0)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>meanQuality</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>4ca73f13284c490d8cdc81b674090b70</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMeanQuality</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>0134305620dcd462968a7e38ed15280d</anchor>
      <arglist>(double meanquality)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>updateMeanQuality</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>732688ad1369771a697e05561fb0c1c4</anchor>
      <arglist>(bool good=false)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>meanRatio</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>269fc20efc356276cbfc6d83f075b366</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setMeanRatio</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>4817cba86989491743c3724dc871d933</anchor>
      <arglist>(double ratio)</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>next</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>928a4f5a371fbd5a3a8d96e1a76a4a74</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>nextTime</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>b07516186a6bf3ca07e32554680bde33</anchor>
      <arglist>(double time, double dflt=-HUGE_VAL) const</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>previous</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c6e9925c308185306210311b6f79c5b0</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>previousTime</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>6646f5f4920a10562592300ace1609f5</anchor>
      <arglist>(double time, double dflt=-HUGE_VAL) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>within</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>6523195ad2eedd91f3c6ca30c82d57e2</anchor>
      <arglist>(double time, double distance) const</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>count</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>1426aeba7c9145b41f17562488ba0ad3</anchor>
      <arglist>(double tbegin, double tend) const</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>count</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>cf7b67426a0a361bd130a808331e912e</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>5d166f471ecee59979f3382176ebf3ff</anchor>
      <arglist>(double tbegin, double tend) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>f4626042dd0eaf6f94beb671bdc5b344</anchor>
      <arglist>(int n) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>66687160ae69ee92e3cb21cc07f4bb21</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>aac35147886c9b5fd66642e0ecb2c6c4</anchor>
      <arglist>(SampleDataD &amp;rate, double width=0.0, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addRate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>49eed234af6a354857e909df2b6b773d</anchor>
      <arglist>(SampleDataD &amp;rate, int &amp;trial, double width=0.0, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyclicRate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>22a7405a5bf846bcdbb0886b25279621</anchor>
      <arglist>(SampleDataD &amp;rate, double width=0.0, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addCyclicRate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>7a2a74f8ab17441afa6f9ee476d8b3d1</anchor>
      <arglist>(SampleDataD &amp;rate, int &amp;trial, double width=0.0, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>463b5dfd7694c32014d17741a83123b6</anchor>
      <arglist>(SampleDataD &amp;rate, const Kernel &amp;kernel, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addRate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>6c71af778959e8f907a572e4ee4a7296</anchor>
      <arglist>(SampleDataD &amp;rate, int &amp;trial, const Kernel &amp;kernel, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyclicRate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>a78c2a997ba670d3cdd87b41989fecac</anchor>
      <arglist>(SampleDataD &amp;rate, const Kernel &amp;kernel, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addCyclicRate</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>44d5e6d13dbabd6b61d859a5cf6a1ba9</anchor>
      <arglist>(SampleDataD &amp;rate, int &amp;trial, const Kernel &amp;kernel, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>interval</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>a7c31631a91058290b27cb8811708c0b</anchor>
      <arglist>(double tbegin, double tend, double *sd=0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>interval</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>13ab16c5cba0207ddad183cb2773a6e8</anchor>
      <arglist>(double time, double *sd=0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>interval</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>b80687c358dd285adc650a54a29ee429</anchor>
      <arglist>(int n, double *sd) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>intervalAt</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>f48fcf83626c2fb048eea822e10b72d0</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>interval</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>934c959cc440878ddc58c1dcd3701366</anchor>
      <arglist>(SampleDataD &amp;intervals, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addInterval</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>7a100aed3a92a1860f93e5d87d8c667a</anchor>
      <arglist>(SampleDataD &amp;intervals, int &amp;trial, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyclicInterval</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>0f538f581d04f3335bef0a965c7ec6a7</anchor>
      <arglist>(SampleDataD &amp;intervals, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addCyclicInterval</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>e15b7b0e1d8192f867f9fc2a08acdc44</anchor>
      <arglist>(SampleDataD &amp;intervals, int &amp;trial, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frequency</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>f4f472e9a1c5d30f53834911b5e1b5c6</anchor>
      <arglist>(double tbegin, double tend, double *sd=0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frequency</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>5958afd78c5515a7196e95970a8fde7f</anchor>
      <arglist>(double time, double *sd=0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frequency</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>545f5a686ba61b09a983c36d9261fcfb</anchor>
      <arglist>(int n, double *sd=0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frequencyAt</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>649339f1c7500f287aec81315072b5e5</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>frequency</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>d6e30b1deb6f75dff3cda99b2564837c</anchor>
      <arglist>(SampleDataD &amp;rate, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addFrequency</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>4c99fd50c55fe73a8deabf55fbddf5b8</anchor>
      <arglist>(SampleDataD &amp;rate, int &amp;trial, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addFrequency</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>5bae3b7df4e5adc394b32b4f7ffd32bb</anchor>
      <arglist>(SampleDataD &amp;rate, SampleDataD &amp;period, int &amp;trial, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyclicFrequency</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>5b2fc1e4feb3d2f1e7f51d317aa42c77</anchor>
      <arglist>(SampleDataD &amp;rate, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addCyclicFrequency</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c14e504b75195083fd12351cd085c5d5</anchor>
      <arglist>(SampleDataD &amp;rate, int &amp;trial, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addCyclicFrequency</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>76353ab926f30a0e007962a3d528540f</anchor>
      <arglist>(SampleDataD &amp;rate, SampleDataD &amp;period, int &amp;trial, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>intervalHistogram</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>cb6b2eb751c284fcece799e3928480a4</anchor>
      <arglist>(double tbegin, double tend, SampleDataD &amp;hist) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addIntervalHistogram</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>aefce905e164ee322263aac384f7a0f5</anchor>
      <arglist>(double tbegin, double tend, SampleDataD &amp;hist) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>serialCorr</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c6367e917b566bd4441d2c413fb056cd</anchor>
      <arglist>(double tbegin, double tend, ArrayD &amp;sc) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>fano</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>9caa0cf2c192fcb4b3e637fcd3ebb8d2</anchor>
      <arglist>(double tbegin, double tend, SampleDataD &amp;ff) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>locking</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>371345693a3c3fee557ab3814fe4c7f0</anchor>
      <arglist>(double tbegin, double tend, double period) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>vectorStrength</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>66ee84376e56d4775023a53af8ec8b5b</anchor>
      <arglist>(double tbegin, double tend, double period) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>vectorPhase</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>2496d4bdd7cf2db9c44d4fbb0fc0cb79</anchor>
      <arglist>(double tbegin, double tend, double period) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>average</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>18aab5b18913037f3efcd1667a1219ee</anchor>
      <arglist>(double tbegin, double tend, const SampleDataD &amp;trace, SampleDataD &amp;ave) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>average</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>6720ea07410f35888fd43fde6049ceaa</anchor>
      <arglist>(double tbegin, double tend, const SampleDataD &amp;trace, SampleDataD &amp;ave, SampleDataD &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>spectrum</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>ec1bed0cbe7487b71d5333c003ead3fb</anchor>
      <arglist>(double tbegin, double tend, double step, SampleDataD &amp;psd) const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>coherence</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>cdb72f6e246143a92bedc8198e678614</anchor>
      <arglist>(const SampleDataD &amp;stimulus, SampleDataD &amp;c) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>coherence</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>8e9e447b96a1fae539f174d1a755b37d</anchor>
      <arglist>(const EventData &amp;e, double tbegin, double tend, double step, SampleDataD &amp;c) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>latency</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>4dc6f384bf0bbb6ceae1d7dc044098c7</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>poisson</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>a35a7938b82bf9468c1fa976ccf2740e</anchor>
      <arglist>(double rate, double refract, double duration, RandomBase &amp;random=numerics::rnd)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>saveText</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>ef679bc9cbda756ff442efb6bc9e482b</anchor>
      <arglist>(ostream &amp;os, double tfac=1.0, int width=0, int prec=5, char frmt=&apos;g&apos;, const string &amp;nospikes=&quot;&quot;) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>savePoint</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>a3efeec4c008fd1f478aa2433528c892</anchor>
      <arglist>(ostream &amp;os, double y=0.0, double tfac=1.0, int width=0, int prec=5, char frmt=&apos;g&apos;, const string &amp;noevents=&quot;&quot;, double noy=-1.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>saveStroke</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>54e66ee3aba8e686126bcb611836ed45</anchor>
      <arglist>(ostream &amp;os, int offs=0, double tfac=1.0, int width=0, int prec=5, char frmt=&apos;g&apos;, double lower=0.1, double upper=0.9, const string &amp;noevents=&quot;&quot;, double noy=-1.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>saveBox</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>126f3e5a37105ed74e420626b62b368a</anchor>
      <arglist>(ostream &amp;os, double bin, int offs=0, double tfac=1.0, int width=0, int prec=5, char frmt=&apos;g&apos;, double lower=0.1, double upper=0.9, const string &amp;noevents=&quot;&quot;, double noy=-1.0) const</arglist>
    </member>
    <member kind="friend" protection="private">
      <type>friend class</type>
      <name>EventIterator</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>c57a4aa458c0a4baca8e36553e950298</anchor>
      <arglist></arglist>
    </member>
    <member kind="friend">
      <type>friend ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classEventData.html</anchorfile>
      <anchor>70ccf3c40ec161e0100eada7701b3d25</anchor>
      <arglist>(ostream &amp;str, const EventData &amp;events)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>EventFrequencyIterator</name>
    <filename>classEventFrequencyIterator.html</filename>
    <base>EventIterator</base>
    <member kind="function">
      <type></type>
      <name>EventFrequencyIterator</name>
      <anchorfile>classEventFrequencyIterator.html</anchorfile>
      <anchor>047911994b322120e65bcd06a8196996</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventFrequencyIterator</name>
      <anchorfile>classEventFrequencyIterator.html</anchorfile>
      <anchor>54b7e098f24a7f30f90115c66000e409</anchor>
      <arglist>(const EventData &amp;ed, long index)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventFrequencyIterator</name>
      <anchorfile>classEventFrequencyIterator.html</anchorfile>
      <anchor>8c95678cce9f917656cc0a224be13081</anchor>
      <arglist>(const EventFrequencyIterator &amp;p)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventFrequencyIterator</name>
      <anchorfile>classEventFrequencyIterator.html</anchorfile>
      <anchor>bbb5cb9a15c4553bf2e811a38ce75924</anchor>
      <arglist>(const EventIterator &amp;p)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~EventFrequencyIterator</name>
      <anchorfile>classEventFrequencyIterator.html</anchorfile>
      <anchor>d59bdd04ad18fe8f8ef9acc36287ae4c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!</name>
      <anchorfile>classEventFrequencyIterator.html</anchorfile>
      <anchor>b2635bf7d2822011bb5d3f8d7a26dc2d</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator *</name>
      <anchorfile>classEventFrequencyIterator.html</anchorfile>
      <anchor>bb5688696b6ccb5b6ebaaca8c106601c</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator[]</name>
      <anchorfile>classEventFrequencyIterator.html</anchorfile>
      <anchor>dea538920edd3d97162bbfa2f6a450bf</anchor>
      <arglist>(long n) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>EventIterator</name>
    <filename>classEventIterator.html</filename>
    <member kind="function">
      <type></type>
      <name>EventIterator</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>b0ec754b8f18807f414668337b11e9d7</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventIterator</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>f6cc6cf5804a942e8543875ec3be2e92</anchor>
      <arglist>(const EventData &amp;ed, long index)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventIterator</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>3a4e0e44baca79e3af1c2158e771b2ab</anchor>
      <arglist>(const EventIterator &amp;p)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~EventIterator</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>e53a4a23e60a431596c78087f63d2703</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>EventIterator &amp;</type>
      <name>operator=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>85b0a3e9bd69d9b827e7c7ad6139abae</anchor>
      <arglist>(const EventIterator &amp;p)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>b583a011d8e4e461d75bd8a8072fee22</anchor>
      <arglist>(const EventIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>7a033c59af639a29d6376a2a8d6d39c2</anchor>
      <arglist>(const EventIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>49f4db33bd86a5a9556e1b5995981159</anchor>
      <arglist>(const EventIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>c0154ed2534f7740cc5a717089c51c9b</anchor>
      <arglist>(const EventIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>8b8d803eb9205def83996347195dffed</anchor>
      <arglist>(const EventIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>2dd0f3d93566030c6762f57f818e7d3d</anchor>
      <arglist>(const EventIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>86a2de2f49f57db4d6929a677889f03c</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator++</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>ea7d6b847afd4c797fdd9c9eb5082c3e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator--</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>04e0c455af6484e9e299210d1926387a</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator+=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>b935b6a8f2d3065809e06d6a3e2c1c63</anchor>
      <arglist>(unsigned int incr)</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator+=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>c646fc6d9daf96a4a92c3e48264796f5</anchor>
      <arglist>(signed int incr)</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator+=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>28ef9f88fae08a45185d1c6f0cd4191c</anchor>
      <arglist>(unsigned long incr)</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator+=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>266102faa70f6bf4ce83402b2c3644e1</anchor>
      <arglist>(signed long incr)</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator+=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>3f71258b5cb53a199f5c62fdb4f1628e</anchor>
      <arglist>(double time)</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator-=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>24aa8188ae046ad3c4b4150cfd0fc853</anchor>
      <arglist>(unsigned int decr)</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator-=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>73b8dd87e434e6496724beae99ba53a7</anchor>
      <arglist>(signed int decr)</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator-=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>78c16e0be849893a4b9d9e844bf4e0ad</anchor>
      <arglist>(unsigned long decr)</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator-=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>5c50ea99921d4a5a5c2f183f5eb6c8c6</anchor>
      <arglist>(signed long decr)</arglist>
    </member>
    <member kind="function">
      <type>const EventIterator &amp;</type>
      <name>operator-=</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>9c5f3d71b2318c552cca6f0421cd4417</anchor>
      <arglist>(double time)</arglist>
    </member>
    <member kind="function">
      <type>EventIterator</type>
      <name>operator+</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>7a51ee0c5152af4d4d20dd9bd34d8645</anchor>
      <arglist>(unsigned int incr) const</arglist>
    </member>
    <member kind="function">
      <type>EventIterator</type>
      <name>operator+</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>5ebbfc54fe9e767462bd0c229fe76eed</anchor>
      <arglist>(signed int incr) const</arglist>
    </member>
    <member kind="function">
      <type>EventIterator</type>
      <name>operator+</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>b19cf0272b937831c97f1d69ceaafee9</anchor>
      <arglist>(unsigned long incr) const</arglist>
    </member>
    <member kind="function">
      <type>EventIterator</type>
      <name>operator+</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>ac28f1f877e97b7399f56442cf12b229</anchor>
      <arglist>(signed long incr) const</arglist>
    </member>
    <member kind="function">
      <type>EventIterator</type>
      <name>operator+</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>0d962d8b4cdcbdb7748ce0b234f1e710</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>EventIterator</type>
      <name>operator-</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>e80ae90ce10cb00206868b305517848a</anchor>
      <arglist>(unsigned int decr) const</arglist>
    </member>
    <member kind="function">
      <type>EventIterator</type>
      <name>operator-</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>cb5d9afe46bdacdfb8a47f249faf4dc5</anchor>
      <arglist>(signed int decr) const</arglist>
    </member>
    <member kind="function">
      <type>EventIterator</type>
      <name>operator-</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>9a8e7c09b3963ded76253c8e5811b75d</anchor>
      <arglist>(unsigned long decr) const</arglist>
    </member>
    <member kind="function">
      <type>EventIterator</type>
      <name>operator-</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>566d72dba81410e4bf3f5fa2b6495453</anchor>
      <arglist>(signed long decr) const</arglist>
    </member>
    <member kind="function">
      <type>EventIterator</type>
      <name>operator-</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>1b719aa5bfce005f08be62eabc82f467</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>operator-</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>dbd0ab1037bc48e401dd0e1d648711a8</anchor>
      <arglist>(const EventIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator *</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>5daece8e006e3d4356af5c29867946a0</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator[]</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>b798287a5042dd1b4edceb457873ff63</anchor>
      <arglist>(long n) const</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>index</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>fbccd2c06b28292b9a3d7d8ecba65ce9</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>time</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>fea22e85d3f2d09701f4f0184faf7b52</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>events</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>4886da47deffe6562b986320c4e9ef10</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>long</type>
      <name>Index</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>921bc5ff8298d4b5e18c98080f2cc764</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>const EventData *</type>
      <name>ED</name>
      <anchorfile>classEventIterator.html</anchorfile>
      <anchor>a885acddc3acb05e29546f3fd8fba5d0</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>EventList</name>
    <filename>classEventList.html</filename>
    <member kind="typedef">
      <type>vector&lt; EventData * &gt;</type>
      <name>EL</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>1c7fcc5c9ad9b30444ffdd22dacdedb5</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>EL::iterator</type>
      <name>iterator</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>a4f9df9bdfebcfc78234f64cc7c2f138</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>EL::const_iterator</type>
      <name>const_iterator</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>0202333d4d71b4ed36fa0030c92644be</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>4153b4ae168dff0c1a8b56b077e029fc</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>fbec089ee4abb5fb02ba0fb79879f783</anchor>
      <arglist>(const EventList &amp;el)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>3a9e8844e282abc4e347cf54fd46e049</anchor>
      <arglist>(const EventList &amp;el, double tbegin, double tend)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>3cc29b5ac5cbc3cdbae7a7a6cd5767ac</anchor>
      <arglist>(const EventList &amp;el, double tbegin, double tend, double tref)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>fa2882fa159f891945bd49f1d92ccb79</anchor>
      <arglist>(const EventData &amp;events)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>04be55c9b2d25db911d97a337d107d13</anchor>
      <arglist>(const EventData &amp;events, double tbegin, double tend)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>905e8ca5ae2ccfc207a4400d6ca09936</anchor>
      <arglist>(const EventData &amp;events, double tbegin, double tend, double tref)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>7d01f52c1d3bfd89bde3d075b471dd40</anchor>
      <arglist>(const EventData &amp;events, const EventData &amp;times, double tend)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>5a9e925ac80de7d02d2d02724df623d8</anchor>
      <arglist>(const EventData &amp;events, const EventData &amp;times, double tbegin, double tend)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>ba0ed51e4f1226a96fcb9c520d386d10</anchor>
      <arglist>(EventData *events, bool own=false)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>a2cc33e9eb942ee4b592b528fb6029db</anchor>
      <arglist>(int n, int m=0, bool sizebuffer=false, bool widthbuffer=false)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~EventList</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>95ea7f68eed45dca6735d00dd3e29475</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>size</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>51bce12078f09105080d3565b6f7eab2</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>empty</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>cc7ad407253218a166ab616e4c44d01d</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>5af733adc27bec7300a7ce5e3225c7e0</anchor>
      <arglist>(int n, int m=0, bool sizebuffer=false, bool widthbuffer=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>87fda27cc419fa0fd925abba7cd2d8fc</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>capacity</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>9b662936d656d05f877bf6f212fb3b6a</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reserve</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>69f778dca5f1775821e5ad42f16a2101</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>EventList &amp;</type>
      <name>operator=</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>4de93040f35a1d861b16c14d965a0789</anchor>
      <arglist>(const EventList &amp;el)</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>operator[]</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>1452932d25b4d6082801a6275be1ac7e</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>EventData &amp;</type>
      <name>operator[]</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>7fe680d80185498258500ec805082dad</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>front</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>e2605bc293c6181fab99f3bdc775eb69</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>EventData &amp;</type>
      <name>front</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>e1d64d19824bda69625913243618cd64</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>back</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>298d8565457272992e310195bde6d1b8</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>EventData &amp;</type>
      <name>back</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>6fa4243e738a0941608107ae5484ec28</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const EventData &amp;</type>
      <name>operator[]</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>1e29fb8bb3a6d4d6c7aa10454bd163c0</anchor>
      <arglist>(const string &amp;ident) const</arglist>
    </member>
    <member kind="function">
      <type>EventData &amp;</type>
      <name>operator[]</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>dbdab035f4ee1655b49c0422fd2cb184</anchor>
      <arglist>(const string &amp;ident)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>index</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>66b3f1cbaf791a50ac8a6352b9a6eeee</anchor>
      <arglist>(const string &amp;ident) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>1b88cf623de0c3eed57cc470a0e5da53</anchor>
      <arglist>(const EventData &amp;events)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>8d544fff3b7626181fb6fb97377407a7</anchor>
      <arglist>(const EventData &amp;events, double tbegin, double tend)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>194817e9edffcfd55b4c37516aa8dccb</anchor>
      <arglist>(const EventData &amp;events, double tbegin, double tend, double tref)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>c490d2b5fbeec430faaf4547acc8071b</anchor>
      <arglist>(const ArrayD &amp;events, double tbegin=-HUGE_VAL, double tend=HUGE_VAL, double stepsize=0.0001)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>f005d26005f8aab2b06d61e663a18991</anchor>
      <arglist>(const EventData &amp;events, const EventData &amp;times, double tend)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>2033ff40ac32ecd9fba0968b1f4bce3b</anchor>
      <arglist>(const EventData &amp;events, const EventData &amp;times, double tbegin, double tend)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>642fe6818f13d5fa91a37bd44e43e835</anchor>
      <arglist>(const EventList &amp;el)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>6e049490ade8af31914bf8537bf1160e</anchor>
      <arglist>(const EventList &amp;el, double tbegin, double tend)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>3b5eae34eb752ff9a7e91d5b52dbbd24</anchor>
      <arglist>(const EventList &amp;el, double tbegin, double tend, double tref)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>dd36a8948c3a6241b272330c91dc0625</anchor>
      <arglist>(int m, bool sizebuffer=false, bool widthbuffer=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>3703ac73cf739991e7cf93e022b47909</anchor>
      <arglist>(int n, int m, bool sizebuffer=false, bool widthbuffer=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>e6928b2a3282711d197ea86d5db06a82</anchor>
      <arglist>(EventData *events, bool own=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>add</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>dc2b709ecb905479078de332b4a033fe</anchor>
      <arglist>(const EventData *events, bool own=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>erase</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>063b9183739ae01bdeac6723226a2913</anchor>
      <arglist>(int index)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clearBuffer</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>a3deea38921c5f6871d8d6fba419b616</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>begin</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>679938fb8443003ca8e43801b3f61512</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>begin</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>44e275d5e515205d243bc496ec57e6a4</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>end</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>3f02287837742998ec8e9802c41b4333</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>end</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>70c8c1729b585333efcfb3cb4ce95ba4</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOffset</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>c31c0c52e5fcadfbaa10f518b8197d33</anchor>
      <arglist>(double offset)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setLength</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>0acc2c47157e946a301e72357dcbc90a</anchor>
      <arglist>(double duration)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setStepsize</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>fa4e431d9b6f0889a982ef2f106df91b</anchor>
      <arglist>(double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRangeFront</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>579dc869b4a89ca11140597230e61fa6</anchor>
      <arglist>(double front)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRangeBack</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>17bf4ffd2b342e8c283b07e5e88a3543</anchor>
      <arglist>(double back)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setSignalTime</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>1c4a5852caea1ff7334319f934d84740</anchor>
      <arglist>(double s)</arglist>
    </member>
    <member kind="function">
      <type>const EventList &amp;</type>
      <name>operator+=</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>b27ba78048b2e354c24f5170202ca1e3</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const EventList &amp;</type>
      <name>operator-=</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>ad393217db3caa80d1500b557c57e125</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const EventList &amp;</type>
      <name>operator *=</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>31d7b615e994f644f39d220ed83bb4ef</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const EventList &amp;</type>
      <name>operator/=</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>b5e1a68d916a23882ca11eea480e61a4</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>count</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>a7d6629dab054c8b01f6e861c0915c76</anchor>
      <arglist>(double tbegin, double tend, double *sd=0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>totalCount</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>4d661ca297a2aef8f1697025820d3e6f</anchor>
      <arglist>(double tbegin, double tend) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>ae346ddb2058c2f27f04cb724fb6f630</anchor>
      <arglist>(double tbegin, double tend, double *sd=0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>10465ebb2f484b64cb940605b2a279c7</anchor>
      <arglist>(SampleDataD &amp;rate, double width=0.0, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addRate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>db7f8c0f1741c060299eb0f2fff1f3b6</anchor>
      <arglist>(SampleDataD &amp;rate, int &amp;trial, double width=0.0, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyclicRate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>02219233087995bbb7008d10088e7fbb</anchor>
      <arglist>(SampleDataD &amp;rate, double width=0.0, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addCyclicRate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>e6f6bdc7467c661e7edbbfaa607885d7</anchor>
      <arglist>(SampleDataD &amp;rate, int &amp;trial, double width=0.0, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>fbc2573ebcb210c0e17c1da53db4964a</anchor>
      <arglist>(SampleDataD &amp;rate, const Kernel &amp;kernel, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>bd4aa3d37960a32773e4659f6f46e6ff</anchor>
      <arglist>(SampleDataD &amp;rate, SampleDataD &amp;ratesd, const Kernel &amp;kernel, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addRate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>f7393e51d381f09f7eb4f28f7f2fd7e5</anchor>
      <arglist>(SampleDataD &amp;rate, int &amp;trial, const Kernel &amp;kernel, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyclicRate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>d0ebc12047396ab04154814148a63477</anchor>
      <arglist>(SampleDataD &amp;rate, const Kernel &amp;kernel, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyclicRate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>e0c2ea04b69efcf25ac6c24c8f737af8</anchor>
      <arglist>(SampleDataD &amp;rate, SampleDataD &amp;ratesd, const Kernel &amp;kernel, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>addCyclicRate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>efccf38d16adc112123cd1aac052f744</anchor>
      <arglist>(SampleDataD &amp;rate, int &amp;trial, const Kernel &amp;kernel, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>intervals</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>dba41ca43f6d26c33cffeecdde3b2fd5</anchor>
      <arglist>(SampleData&lt; ArrayD &gt; &amp;intervals, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>intervalsAt</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>573fe7ff7eb261be0d168f4c1f988f67</anchor>
      <arglist>(double time, ArrayD &amp;intervals) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>interval</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>99fb2f491fcbaa805f7cdb4a3a7299d7</anchor>
      <arglist>(double tbegin, double tend) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>interval</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>a4cdf281b1811a4b8064e494d4946445</anchor>
      <arglist>(double tbegin, double tend, double &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>interval</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>efa8246269234d65897b98f44b28bb73</anchor>
      <arglist>(SampleDataD &amp;intervals, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>interval</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>ce98c026bebfbf60312a6203f5e721e7</anchor>
      <arglist>(SampleDataD &amp;intervals, SampleDataD &amp;sd, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>intervalCV</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>219d4bc90b9ed2876dcb55a1bbe87523</anchor>
      <arglist>(SampleDataD &amp;intervals, SampleDataD &amp;cv, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyclicInterval</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>aef4bc53cc9ebc1e6a12b09c63ffc4b4</anchor>
      <arglist>(SampleDataD &amp;intervals, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>intervalAt</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>46daaa51bb26efbafdf4c2703920639a</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>intervalAt</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>468bb6aea96ccb54b5a428a8f16b294c</anchor>
      <arglist>(double time, double &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frequency</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>69ffda4d708250bb89f970f18a0f3d31</anchor>
      <arglist>(double tbegin, double tend) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frequency</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>70e4af4eb5c2ae2b1bdb81149c005902</anchor>
      <arglist>(double tbegin, double tend, double &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>frequency</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>34843a780f71450ce8d8c83515ceea62</anchor>
      <arglist>(SampleDataD &amp;rate, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>frequency</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>626522fe9272a120f3a56d60ea44af4e</anchor>
      <arglist>(SampleDataD &amp;rate, SampleDataD &amp;sd, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>frequencyCV</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>c4916037d31a1916a551eb7fe157c7a5</anchor>
      <arglist>(SampleDataD &amp;rate, SampleDataD &amp;cv, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyclicFrequency</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>e2d6a2b5dd96ae51d046fb1ba004a030</anchor>
      <arglist>(SampleDataD &amp;rate, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>cyclicFrequency</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>61deb662ca7d3eb1dbbeaf20a59a61ad</anchor>
      <arglist>(SampleDataD &amp;rate, SampleDataD &amp;sd, double time=0.0) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frequencyAt</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>0a528b449d988828a8e83baec99f642c</anchor>
      <arglist>(double time) const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frequencyAt</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>6540a558118e8307799efb4ab88c489c</anchor>
      <arglist>(double time, double &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>intervalHistogram</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>7131a44e468332242f5511b97ea6eda1</anchor>
      <arglist>(double tbegin, double tend, SampleDataD &amp;hist) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>directIntervalHistogram</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>6c6a7aea1edc4b71a48110c95ad7dd67</anchor>
      <arglist>(double time, SampleDataD &amp;hist) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>correctedIntervalHistogram</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>4589f2f661406573fe5899c1059775b8</anchor>
      <arglist>(double time, SampleDataD &amp;hist) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>serialCorr</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>20d361e92df59cc00b2f280066613611</anchor>
      <arglist>(double tbegin, double tend, ArrayD &amp;sc) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>serialCorr</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>492b7ab5821a1cbf2c07cf5fb81b3140</anchor>
      <arglist>(double tbegin, double tend, ArrayD &amp;sc, ArrayD &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>locking</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>c55800b22043dc18a0d5d3da333ee309</anchor>
      <arglist>(double tbegin, double tend, double period) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>locking</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>1cbcba2d5a0f22b4a525ea412937ce13</anchor>
      <arglist>(double tbegin, double tend, double period, double &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>vectorStrength</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>897aff136f11a7ebe4c915976736fcfe</anchor>
      <arglist>(double tbegin, double tend, double period) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>vectorStrength</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>6d7ef22ab34976a3d45317db12e21ff0</anchor>
      <arglist>(double tbegin, double tend, double period, double &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>vectorPhase</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>ee8acc8f44624399e40d14b9af5f18b6</anchor>
      <arglist>(double tbegin, double tend, double period) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>vectorPhase</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>b392fd8340825530b0ad5cb6f81c5b8c</anchor>
      <arglist>(double tbegin, double tend, double period, double &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>correlation</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>a8a970bd6e22a32f9aecb86255828b39</anchor>
      <arglist>(double tbegin, double tend, const Kernel &amp;kernel, double dt, double &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>correlation</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>4345b94dc5b127a4d47ef400e7eb338f</anchor>
      <arglist>(double tbegin, double tend, const Kernel &amp;kernel, double dt=0.001) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>reliability</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>b70dca7a16dd747b9cfca053804fb905</anchor>
      <arglist>(double tbegin, double tend, const Kernel &amp;kernel, double dt, double &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>reliability</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>b1f285dd578117db7adb06959e51e3ad</anchor>
      <arglist>(double tbegin, double tend, const Kernel &amp;kernel, double dt=0.001) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>coincidenceRate</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>960023ffbc8af6a15b974e94197467af</anchor>
      <arglist>(SampleDataD &amp;rate, SampleDataD &amp;ratesd, const Kernel &amp;kernel)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>average</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>3587e12817af8546bc305f4f6adc9b0f</anchor>
      <arglist>(double tbegin, double tend, const SampleDataD &amp;trace, SampleDataD &amp;ave) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>average</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>0d751033f662e4c74d6ea67abe962a5c</anchor>
      <arglist>(double tbegin, double tend, const SampleDataD &amp;trace, SampleDataD &amp;ave, SampleDataD &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>spectrum</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>c20ba6651bb0c97f8a8c0c37863cee87</anchor>
      <arglist>(double tbegin, double tend, double step, SampleDataD &amp;psd) const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>spectrum</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>e5edc3ed9fd5deb0b2e7f00adb87ac57</anchor>
      <arglist>(double tbegin, double tend, double step, SampleDataD &amp;psd, SampleDataD &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>coherence</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>c6711f1181975431de1dbd6b5bc22cb5</anchor>
      <arglist>(const SampleDataD &amp;stimulus, SampleDataD &amp;c) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>coherence</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>f3eaab6bd35cfa6f60421d868d177ac6</anchor>
      <arglist>(const SampleDataD &amp;stimulus, SampleDataD &amp;c, SampleDataD &amp;sd) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>coherence</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>c170928f88ba4d2203d55899e7826206</anchor>
      <arglist>(double tbegin, double tend, double step, SampleDataD &amp;c) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>coherence</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>7f215f4dbf3770f56a06fd6c5f3c5d75</anchor>
      <arglist>(double tbegin, double tend, double step, SampleDataD &amp;c, SampleDataD &amp;sd) const </arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>latency</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>2a4f6aa44352ad70777873af4f6da274</anchor>
      <arglist>(double time, double *sd=0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sum</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>85dd484d6a3b45f507f4018fb0a6e376</anchor>
      <arglist>(EventData &amp;all)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>sync</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>87ca21ce328a133d6698e4320ffd254f</anchor>
      <arglist>(EventData &amp;s, double bin, double p=1.0, bool keep=false)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>poisson</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>57eaf74466038b2576f2370dd92e10c3</anchor>
      <arglist>(int trials, double rate, double refract, double duration, RandomBase &amp;random=numerics::rnd)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>saveText</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>f6d653ad0d26e88c88e5dd787e4c20b4</anchor>
      <arglist>(ostream &amp;os, double tfac=1.0, int width=0, int prec=5, char frmt=&apos;g&apos;, int sep=1, const string &amp;noevents=&quot;&quot;) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>savePoint</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>88a20cdbc120f12eca780ff9d54d1cab</anchor>
      <arglist>(ostream &amp;os, double y=0.0, double tfac=1.0, int width=0, int prec=5, char frmt=&apos;g&apos;, int sep=1, const string &amp;noevents=&quot;&quot;, double noy=-1.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>saveStroke</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>c7545592e282e742d57a580f1c076cb7</anchor>
      <arglist>(ostream &amp;os, int offs=0, double tfac=1.0, int width=0, int prec=5, char frmt=&apos;g&apos;, int sep=0, double lower=0.1, double upper=0.9, const string &amp;noevents=&quot;&quot;, double noy=-1.0) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>saveBox</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>065bda59571d12d7bf7da7caf958de4c</anchor>
      <arglist>(ostream &amp;os, double bin, int offs=0, double tfac=1.0, int width=0, int prec=5, char frmt=&apos;g&apos;, int sep=0, double lower=0.1, double upper=0.9, const string &amp;noevents=&quot;&quot;, double noy=-1.0) const</arglist>
    </member>
    <member kind="friend">
      <type>friend ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classEventList.html</anchorfile>
      <anchor>127e14861a43b0eee9da8f6ce567c5d1</anchor>
      <arglist>(ostream &amp;str, const EventList &amp;events)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>EventSizeIterator</name>
    <filename>classEventSizeIterator.html</filename>
    <base>EventIterator</base>
    <member kind="function">
      <type></type>
      <name>EventSizeIterator</name>
      <anchorfile>classEventSizeIterator.html</anchorfile>
      <anchor>878f03501f45573704d46d9ee07e785f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventSizeIterator</name>
      <anchorfile>classEventSizeIterator.html</anchorfile>
      <anchor>930f1e8b0612495cdebd05a61900ade2</anchor>
      <arglist>(const EventData &amp;ed, long index)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventSizeIterator</name>
      <anchorfile>classEventSizeIterator.html</anchorfile>
      <anchor>cd2856d4675dc3544394b2a6ddf7e31b</anchor>
      <arglist>(const EventSizeIterator &amp;p)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>EventSizeIterator</name>
      <anchorfile>classEventSizeIterator.html</anchorfile>
      <anchor>3d8f24504222d45a209be4166f36317f</anchor>
      <arglist>(const EventIterator &amp;p)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~EventSizeIterator</name>
      <anchorfile>classEventSizeIterator.html</anchorfile>
      <anchor>134d6614867e4ff86cfab34205f117d9</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator *</name>
      <anchorfile>classEventSizeIterator.html</anchorfile>
      <anchor>041d286ce364b67ab1d34d7d60598a5d</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator[]</name>
      <anchorfile>classEventSizeIterator.html</anchorfile>
      <anchor>a0cabb6a06be51e011b61eb1c0411717</anchor>
      <arglist>(long n) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>GammaKernel</name>
    <filename>classGammaKernel.html</filename>
    <base>Kernel</base>
    <member kind="function">
      <type></type>
      <name>GammaKernel</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>7e4dfcacc2c7c2e74eeb82ab064bfd17</anchor>
      <arglist>(double stdev, int order=1)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>76da16f73f097f8d6de4deff4aab7ac2</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>value</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>6378d6e1f769e34cf054e0d4ade369b1</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setScale</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>329232b1727addaac22996a719661d8b</anchor>
      <arglist>(double scale)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>mean</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>1ea4ca6e30a6f83757aa0ec72470d4cb</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>stdev</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>a8530a91d4d5c295764cc5ba093bfd80</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setStdev</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>b1b2b3aad9bf631a39936b6010a8a774</anchor>
      <arglist>(double stdev)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>max</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>d02eda9cb11f5b685923faade34a8203</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>order</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>786d34668a31361fb1cda14d966ced8f</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOrder</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>be7e25bd95c31a5ec48302dae6e2591a</anchor>
      <arglist>(int order)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>left</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>3dc663c70b064913a1fb919e8a6f52b4</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>right</name>
      <anchorfile>classGammaKernel.html</anchorfile>
      <anchor>e663902073aaec1860a226a7964d7ca8</anchor>
      <arglist>(void) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>GaussKernel</name>
    <filename>classGaussKernel.html</filename>
    <base>Kernel</base>
    <member kind="function">
      <type></type>
      <name>GaussKernel</name>
      <anchorfile>classGaussKernel.html</anchorfile>
      <anchor>c037fb1f8d54b26875c137da6219a673</anchor>
      <arglist>(double stdev)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classGaussKernel.html</anchorfile>
      <anchor>b74faf35b7fc0a6ec0d4a20dd9f8e5b9</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>value</name>
      <anchorfile>classGaussKernel.html</anchorfile>
      <anchor>c93f817c349ecd79cb5747e020fd3efc</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setScale</name>
      <anchorfile>classGaussKernel.html</anchorfile>
      <anchor>5e7c2c513b79eb8300acabc8b9a5acba</anchor>
      <arglist>(double scale)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>mean</name>
      <anchorfile>classGaussKernel.html</anchorfile>
      <anchor>5fb1d690de5259a0eef91f0cb58ead4d</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>stdev</name>
      <anchorfile>classGaussKernel.html</anchorfile>
      <anchor>f4a3e5ab9debcc1f89ca0688fa2a393e</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setStdev</name>
      <anchorfile>classGaussKernel.html</anchorfile>
      <anchor>0750f7a34341e42816bedfd8fbde2d01</anchor>
      <arglist>(double stdev)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>max</name>
      <anchorfile>classGaussKernel.html</anchorfile>
      <anchor>0924ed091dc45b308cf2cd4bc675ab71</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>left</name>
      <anchorfile>classGaussKernel.html</anchorfile>
      <anchor>7019ff2c82777cacd44a37212e7edcec</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>right</name>
      <anchorfile>classGaussKernel.html</anchorfile>
      <anchor>7e833802e698b597dfad2ef9a5670bef</anchor>
      <arglist>(void) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Kernel</name>
    <filename>classKernel.html</filename>
    <member kind="function">
      <type></type>
      <name>Kernel</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>f5ed5aa776085d809e1850b25cec86cf</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Kernel</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>937df2bd27ea42430c7b0664876a1090</anchor>
      <arglist>(double stdev)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~Kernel</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>8e3be61122eef564563f6a9d9c6508e4</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>4b908106a10e2f79c139cd9d9f7f94ab</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>value</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>d2d018e40c68fb878178e7f21ef2b935</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>scale</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>376ab9fc701b1f8c24cea5e7196e5e92</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setScale</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>c4741429fb29ca529b7f2dc12663cbd0</anchor>
      <arglist>(double scale)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>mean</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>bfcce7d435326161fb4a4b4b313502a9</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>stdev</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>321e841d73eb382f4d2afb46ec81a0c9</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setStdev</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>ec92ddd62f40ef92da68fb417618cb8a</anchor>
      <arglist>(double stdev)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>max</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>dfd479e443b589e4a702928e8dd9a924</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>left</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>250a9fe9637bdd21a333905bea073deb</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>right</name>
      <anchorfile>classKernel.html</anchorfile>
      <anchor>6fc38cc44fbba2ce95e2293d00f7ea9a</anchor>
      <arglist>(void) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LinearRange</name>
    <filename>classLinearRange.html</filename>
    <member kind="typedef">
      <type>double</type>
      <name>value_type</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>2e4d79bc3381a6f9d9a7cd431039ec82</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>double *</type>
      <name>pointer</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>637493dfc53479acf799d4ea069eb334</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>double &amp;</type>
      <name>reference</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>0d158820c4f5bd51420bba0ea9e71507</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const double &amp;</type>
      <name>const_reference</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>c4f82e58d2b66b6007646f237c68d6a6</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>long</type>
      <name>size_type</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>2087adb2b18e789f69b3f16e966f5e36</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>LinearRangeIterator</type>
      <name>const_iterator</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>89f37a027b565f1edfd3bf069d744548</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>f496cb8348897f411df4e34fcccec1bd</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>5ae0282336a4eecbb4c51e97a94d6172</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>e6a73d32d802527e2681312e74d9c264</anchor>
      <arglist>(long n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>9bdb2fbdfd20d63ad87bca26907ad662</anchor>
      <arglist>(int n, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>fadce19e130ab1870b4b1cad853ab108</anchor>
      <arglist>(long n, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>a28e98b036497f6bc7b92cf29f7e52e5</anchor>
      <arglist>(int n, double offset, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>9c046f67b429e94ade1801f184b77aad</anchor>
      <arglist>(long n, double offset, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>1d40f4959e15d20501cbc0e70ae473c3</anchor>
      <arglist>(double r, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>d3d88a562d8716537e5d21f5e2655986</anchor>
      <arglist>(double l, double r, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>cc96526d6f6d11931c49b48f65216880</anchor>
      <arglist>(const LinearRange &amp;r)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~LinearRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>af54c9ccf818ffecc54b92c42d71c99b</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>operator=</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>0263b5f8da421e3c6432eaf1941b5798</anchor>
      <arglist>(const LinearRange &amp;r)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>assign</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>a21cc6d91424dcec305db5485dac0764</anchor>
      <arglist>(const LinearRange &amp;r)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>assign</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>25581c70ea3c532db0c89d868f75ed31</anchor>
      <arglist>(const LinearRange &amp;r, long first, long last=-1)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>assign</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>8da62ce23e220391ac38f6707dc5ca9f</anchor>
      <arglist>(int n, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>assign</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>c24b3d2ba92204f355074a8f847a0870</anchor>
      <arglist>(long n, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>assign</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>b7bb738e152c8400433508a083c4c8c6</anchor>
      <arglist>(int n, double offset, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>assign</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>c6d42b6bae27967172e35a9096f02f3b</anchor>
      <arglist>(long n, double offset, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>assign</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>06b3508a0baf868ffd6b307228dea495</anchor>
      <arglist>(double r, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>assign</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>057b110e63f0d676b396c7eaffa89821</anchor>
      <arglist>(double l, double r, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>copy</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>8d7610c23a72537aaa6c402f10c005af</anchor>
      <arglist>(LinearRange &amp;r) const</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>copy</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>c8b4fcab90814202d55d28953d725ba8</anchor>
      <arglist>(LinearRange &amp;r, long first, long last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>append</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>5a35b91205a75cc424dbbf7ead9cd715</anchor>
      <arglist>(long n)</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>size</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>89d53e1e10b85e038dab60dace8bcb8a</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>empty</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>ad1f67bd3d27a8f89f704ab0fc9f65c0</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>38e7cf4de3ef3569a1efcd1d055356c2</anchor>
      <arglist>(long n)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>6f8fc2f308cd6b18b58ec5c693c7ad55</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>offset</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>1e1603f341d838e743ef1fec9569abb2</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOffset</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>7429240cff09073c700069bddf6f9ccd</anchor>
      <arglist>(double offset)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stepsize</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>88cd209416bc646c990f8c1f4c9c1a1b</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setStepsize</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>6472c84df463b0cc1b7be4aeb0c125ad</anchor>
      <arglist>(double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scaleStepsize</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>c10a86024ec07be8bfd23fe6ff8fd89a</anchor>
      <arglist>(double scale)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRange</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>6ccc3e5632bc485396b00266cd28b52a</anchor>
      <arglist>(double offset, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>length</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>747ceb24f091be63db0a9ad8232a079c</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setLength</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>f1dbbd1c17657496363311d5a9992fb9</anchor>
      <arglist>(double l)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>front</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>2b6c4664daf3e887e8b6459ff33089ea</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFront</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>ca3bd3f372fb71c66b5342ac2b14d580</anchor>
      <arglist>(double front)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>back</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>249047a8e7ba0c6fb8575c3648391955</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setBack</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>b3545b7cf229c2d90246af85dfe4e0a3</anchor>
      <arglist>(double back)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator[]</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>e58dd4aab51de03eec4ab18ec9e5b172</anchor>
      <arglist>(long i) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>at</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>480e3efbf1b61180e3268b9e16b10da6</anchor>
      <arglist>(long i) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>pos</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>d84727b1a71541ad7e8a4a286464766c</anchor>
      <arglist>(long i) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>interval</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>fa5ec92481f2bff61cdde92912f314dd</anchor>
      <arglist>(int indices) const </arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>index</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>47ccd22a75f9e9b160c3720276d953bd</anchor>
      <arglist>(double pos) const</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>indices</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>a21b1531a0efc8502de12967d862a2d8</anchor>
      <arglist>(double iv) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>contains</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>4b8e3da9e349030786f053be14e64329</anchor>
      <arglist>(double pos) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>cb0d5d3eeb8ff77bdc5632e5f97ccba4</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pop</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>0555aa3b2469bac613be3df30e8ae5a2</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>begin</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>1bac1f54178a4ef216c90a0bf0125cdd</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>end</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>81c2f49b77a0a7d00895e6a56ecda094</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>operator=</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>4817b635ad3801a2f3721a54a50e91f4</anchor>
      <arglist>(double val)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>operator+=</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>59abc3d3c4920aa29fe3df88b8d1bb43</anchor>
      <arglist>(double val)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>operator-=</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>cbb7ce7ac2885edb27f08f153c8da3bb</anchor>
      <arglist>(double val)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>operator *=</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>881aca0622ab215db1be5107a84a5fbd</anchor>
      <arglist>(double val)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>operator/=</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>8e4f9c45d79e3fc0149016b4b0d64d75</anchor>
      <arglist>(double val)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>min</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>34c8c3acd86a9eddb17ca968699612fe</anchor>
      <arglist>(long first=0, long last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>minIndex</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>0e9f59628f3a4baae8ddb187e9e5916d</anchor>
      <arglist>(long first=0, long last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>minIndex</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>a91bb4ace83138dee13433d99b8e5854</anchor>
      <arglist>(double &amp;min, long first=0, long last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>max</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>149b98059589b0d83ef0227d878efdda</anchor>
      <arglist>(long first=0, long last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>maxIndex</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>1598cb7df992ef1db2e25bc9803bc1e8</anchor>
      <arglist>(long first=0, long last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>long</type>
      <name>maxIndex</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>db44ebd71c4fe5b7244d6c95b40905e5</anchor>
      <arglist>(double &amp;max, long first=0, long last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMax</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>4a003f2ff4900ccb0c006ad1360cda5d</anchor>
      <arglist>(double &amp;min, double &amp;max, long first=0, long last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxIndex</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>03a99976e37bc141fd6049f990f59e07</anchor>
      <arglist>(long &amp;minindex, long &amp;maxindex, long first=0, long last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxIndex</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>611eef702e0ee2c4711ebd92a7257336</anchor>
      <arglist>(double &amp;min, long &amp;minindex, double &amp;max, long &amp;maxindex, long first=0, long last=-1) const</arglist>
    </member>
    <member kind="friend">
      <type>friend bool</type>
      <name>operator==</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>fc32a386341a3ff939d95789920c0ecd</anchor>
      <arglist>(const LinearRange &amp;r, const LinearRange &amp;b)</arglist>
    </member>
    <member kind="friend">
      <type>friend bool</type>
      <name>operator&lt;</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>5273ba1d168f8d9334add85a3e9f8f7b</anchor>
      <arglist>(const LinearRange &amp;r, const LinearRange &amp;b)</arglist>
    </member>
    <member kind="friend">
      <type>friend LinearRange</type>
      <name>operator+</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>26790c5dc2e010e46962c3463546103d</anchor>
      <arglist>(const LinearRange &amp;r, double val)</arglist>
    </member>
    <member kind="friend">
      <type>friend LinearRange</type>
      <name>operator+</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>e70eaa733a7c1db6a11c94c5436d52ce</anchor>
      <arglist>(double val, const LinearRange &amp;r)</arglist>
    </member>
    <member kind="friend">
      <type>friend LinearRange</type>
      <name>operator-</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>af915afab1f3491c880ebe02b436e924</anchor>
      <arglist>(const LinearRange &amp;r, double val)</arglist>
    </member>
    <member kind="friend">
      <type>friend LinearRange</type>
      <name>operator-</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>400bf498f225c08fbd16da19e0a5fbe6</anchor>
      <arglist>(double val, const LinearRange &amp;r)</arglist>
    </member>
    <member kind="friend">
      <type>friend LinearRange</type>
      <name>operator *</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>f61b3ca1f9eb9a29a7adc21d93cf47f2</anchor>
      <arglist>(const LinearRange &amp;r, double val)</arglist>
    </member>
    <member kind="friend">
      <type>friend LinearRange</type>
      <name>operator *</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>92781f683c0c1fafffb8809967c833a9</anchor>
      <arglist>(double val, const LinearRange &amp;r)</arglist>
    </member>
    <member kind="friend">
      <type>friend LinearRange</type>
      <name>operator/</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>33d4e4896a02ade8ea794447c4aba631</anchor>
      <arglist>(const LinearRange &amp;r, double val)</arglist>
    </member>
    <member kind="friend">
      <type>friend ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classLinearRange.html</anchorfile>
      <anchor>d7d9cd49234c8b20345b66c9fc58b592</anchor>
      <arglist>(ostream &amp;str, const LinearRange &amp;r)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>LinearRangeIterator</name>
    <filename>classLinearRangeIterator.html</filename>
    <member kind="function">
      <type></type>
      <name>LinearRangeIterator</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>fbd02682838ad228d23a46f990464bc4</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRangeIterator</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>0b1fb55db926cdedf206f9e8e8775968</anchor>
      <arglist>(double offset, double stepsize, long index)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRangeIterator</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>a29eca23e317a4396270c682e6bfda00</anchor>
      <arglist>(const LinearRange &amp;r, long index)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>LinearRangeIterator</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>811581da0d048a591ad6f371990ec55e</anchor>
      <arglist>(const LinearRangeIterator &amp;p)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>~LinearRangeIterator</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>e605f8b435c8b77a73746e37673191b6</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>LinearRangeIterator &amp;</type>
      <name>operator=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>89c8d73bc198d1ddf2826629fb086f70</anchor>
      <arglist>(const LinearRangeIterator &amp;p)</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator==</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>f38e5a5f151deda6ef6a9f2501bf7b27</anchor>
      <arglist>(const LinearRangeIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator!=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>39d6ab118d51f6c457478d9ef3f52333</anchor>
      <arglist>(const LinearRangeIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>7a79e1880a621ffb069d47c13ba0f468</anchor>
      <arglist>(const LinearRangeIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>173440ec2712ce1056500cf077d302c9</anchor>
      <arglist>(const LinearRangeIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&lt;=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>ca6b3bd3bb0c87cbabccc9a1a10088f0</anchor>
      <arglist>(const LinearRangeIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>operator&gt;=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>081189960fc8c34c6b7f9784385148af</anchor>
      <arglist>(const LinearRangeIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>const LinearRangeIterator &amp;</type>
      <name>operator++</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>371acadae3598cc4710cd191d01f6ecb</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRangeIterator &amp;</type>
      <name>operator--</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>fbbc663f2996e8587381891a61547766</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRangeIterator &amp;</type>
      <name>operator+=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>06fee4e1f2e863061deb3d4502a7c14a</anchor>
      <arglist>(unsigned int incr)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRangeIterator &amp;</type>
      <name>operator+=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>e80b9442fd17f3330cd3f1248aaabcc7</anchor>
      <arglist>(signed int incr)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRangeIterator &amp;</type>
      <name>operator+=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>d44c7f4338b24cded0591e1f2cc30f3e</anchor>
      <arglist>(unsigned long incr)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRangeIterator &amp;</type>
      <name>operator+=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>08455a8762d7f3100827d1902df6a9b9</anchor>
      <arglist>(signed long incr)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRangeIterator &amp;</type>
      <name>operator-=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>168531d41b842953149ba7370a016f3d</anchor>
      <arglist>(unsigned int decr)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRangeIterator &amp;</type>
      <name>operator-=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>0fc8a2144724725fcbbcd28a11132cd0</anchor>
      <arglist>(signed int decr)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRangeIterator &amp;</type>
      <name>operator-=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>12f68903abec0a881146453bf67bd38d</anchor>
      <arglist>(unsigned long decr)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRangeIterator &amp;</type>
      <name>operator-=</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>0080db3148376ef82e05dda3fc223359</anchor>
      <arglist>(signed long decr)</arglist>
    </member>
    <member kind="function">
      <type>LinearRangeIterator</type>
      <name>operator+</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>0a2d9217d88f2b3791095da71a49d629</anchor>
      <arglist>(unsigned int incr) const</arglist>
    </member>
    <member kind="function">
      <type>LinearRangeIterator</type>
      <name>operator+</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>6cd8a09aac27f8acbe7a06e3cee7290d</anchor>
      <arglist>(signed int incr) const</arglist>
    </member>
    <member kind="function">
      <type>LinearRangeIterator</type>
      <name>operator+</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>557a45090e6ce4cc993d7ed0884258a6</anchor>
      <arglist>(unsigned long incr) const</arglist>
    </member>
    <member kind="function">
      <type>LinearRangeIterator</type>
      <name>operator+</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>3c3db243bd92d2560e72993d31dd9ea9</anchor>
      <arglist>(signed long incr) const</arglist>
    </member>
    <member kind="function">
      <type>LinearRangeIterator</type>
      <name>operator-</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>98c91381f2a9b3121199aa05f9cc6223</anchor>
      <arglist>(unsigned int decr) const</arglist>
    </member>
    <member kind="function">
      <type>LinearRangeIterator</type>
      <name>operator-</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>cac3f7aac7946d3719032de6372fc64d</anchor>
      <arglist>(signed int decr) const</arglist>
    </member>
    <member kind="function">
      <type>LinearRangeIterator</type>
      <name>operator-</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>6be5ed87882105d299ce4f3dbf8857b3</anchor>
      <arglist>(unsigned long decr) const</arglist>
    </member>
    <member kind="function">
      <type>LinearRangeIterator</type>
      <name>operator-</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>5d5d288318ffdea553acf67ea31fb47d</anchor>
      <arglist>(signed long decr) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>operator-</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>4d2923d191cad8ac3d851ba7fab0b3ea</anchor>
      <arglist>(const LinearRangeIterator &amp;p) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator *</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>6ec6fe10c0ce3f5f287c42db58855ee2</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator[]</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>7ca9e6171c1ed443047506606c5e885a</anchor>
      <arglist>(long n) const</arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>int</type>
      <name>Index</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>7894d7f53a54ba9540febcdb494dd729</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>Offset</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>a5550c979418c448c878d3a0bfcb1901</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" protection="protected">
      <type>double</type>
      <name>Stepsize</name>
      <anchorfile>classLinearRangeIterator.html</anchorfile>
      <anchor>215c970c0950190959024aa2f042438f</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Map</name>
    <filename>classMap.html</filename>
    <templarg>T</templarg>
    <base>Array</base>
    <member kind="typedef">
      <type>T</type>
      <name>value_type</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>c4bd85be6b23fbc36f408bce588058b3</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>T *</type>
      <name>pointer</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>bad9ce59dd8be6cf385483ad2b407930</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>T &amp;</type>
      <name>reference</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>c5caaeeb6616c7302510bc95a8164017</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const T &amp;</type>
      <name>const_reference</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>b20801d5b45e49fb4e222dc24c72795e</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>int</type>
      <name>size_type</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>4cdea172dbccaa16fa0ff521b76dcad9</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>T *</type>
      <name>iterator</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>15246629e78b0582af5c2baca21ca5c6</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const T *</type>
      <name>const_iterator</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>c36fab4d7193ba26808f7016f97a71ab</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Map</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>bf1bdb06578f9e6ca0f4a720ee475938</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Map</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>717d09e6a445be8f6d9e786b8dc47bde</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Map</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>fd8d57daf1f99a5fedd427e65f2dcc55</anchor>
      <arglist>(int n, const T &amp;xval, const T &amp;yval)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Map</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>e226fa0666b9ca6c215e9fb1a441a422</anchor>
      <arglist>(const T *x, const T *y, int n)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Map</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>613c0d63918e667be482fea72657ed27</anchor>
      <arglist>(const vector&lt; T &gt; &amp;x, const vector&lt; T &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Map</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>334a812f5961bdf4ce336279d088ea2f</anchor>
      <arglist>(const Array&lt; T &gt; &amp;x, const Array&lt; T &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>Map</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>bc87f22980629468e983943ec429cf92</anchor>
      <arglist>(const Map&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~Map</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>3c0466b8eb6f29e3663640015fe5055b</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>363c0c3ef8afaa43f7605181e3e289fa</anchor>
      <arglist>(const R &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>f420144459de69a46ad955ccb27203d6</anchor>
      <arglist>(const Array&lt; T &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>27db780fa7b8f6493a283c1a3f2e6cf2</anchor>
      <arglist>(const Map&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>79016ae5115c9e1aaef3371a12624658</anchor>
      <arglist>(const T *y, int n, const T &amp;xval=0)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>a9fbbd286d3db594cad234f3506cbb90</anchor>
      <arglist>(const T *x, const T *y, int n)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>ff7bcd802a776d118383895ca58a4f1b</anchor>
      <arglist>(const vector&lt; T &gt; &amp;y, const T &amp;xval=0)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>c7da89530e1f08a4eb9a90fc7e43dc78</anchor>
      <arglist>(const vector&lt; T &gt; &amp;x, const vector&lt; T &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>e5a688605cec255087f765f152aa6d1a</anchor>
      <arglist>(const Array&lt; T &gt; &amp;y, const T &amp;xval=0)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>7227e4a7f53e4e4da50a4672685cea1d</anchor>
      <arglist>(const Array&lt; T &gt; &amp;x, const Array&lt; T &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>b155eb44fdbe2de9b44df6fab54b2fa1</anchor>
      <arglist>(const Map&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>9e6ab59f2e091ddbb4f2c6432164d574</anchor>
      <arglist>(T *y, int n, const T &amp;yval=0)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>bc8eaaa6b02e75282ac38cc56724b0c0</anchor>
      <arglist>(T *x, T *y, int n, const T &amp;xval=0, const T &amp;yval=0)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>951e6778fdbb5b39853ce5a6a86551a0</anchor>
      <arglist>(vector&lt; T &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>99fc1a27ac780a4cbd27778062793fba</anchor>
      <arglist>(vector&lt; T &gt; &amp;x, vector&lt; T &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>7c3a2c1e82aab66507f7dc310da9399f</anchor>
      <arglist>(Array&lt; T &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>5d9a0a38994357a8dcb99687ec34b108</anchor>
      <arglist>(Array&lt; T &gt; &amp;x, Array&lt; T &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>c01dd8ef90418e8e9efbc976e78e8e16</anchor>
      <arglist>(Map&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>b08360e196498c0f6b51b6774155e6f8</anchor>
      <arglist>(const T *x, const T *y, int n)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>91ac4031f8762f2a02ae7fda69450b15</anchor>
      <arglist>(const vector&lt; T &gt; &amp;x, const vector&lt; T &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>4b90858e28724482bf9638e90e92654f</anchor>
      <arglist>(const Array&lt; T &gt; &amp;x, const Array&lt; T &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>const Map&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>0019d08752786fd5810993fef1d97098</anchor>
      <arglist>(const Map&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>size</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>50530368f8d48cf5b5670642c7badcab</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>empty</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>3e92214441a0cad7c98b96dfd7516ba2</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>f1f22f10a427a756615dfc712ebeea1b</anchor>
      <arglist>(int n, const T &amp;val=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>7efe3afc69b7d70b9163dab1230435ee</anchor>
      <arglist>(int n, const T &amp;xval, const T &amp;yval)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>clear</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>f72e8292e013a40cdc10e2585e3bdbaa</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>capacity</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>9b3ce2c42409448424d624b7d69da79e</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>reserve</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>4816b70dfd1bae60d2723d96a3952eb8</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>free</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>0edbca80fb366c15db4139f064030866</anchor>
      <arglist>(int n=0)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>x</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>7e0a172cd04d29cda005771839b8b79d</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>x</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>8d72d265c9560b4b70dd65fa12b03139</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>y</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>4d3b10b10e157a2e8f3c3c01d45f5daa</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>y</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>06031df8fe5066b1bbadcffefab786ea</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>x</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>7c6c9ced66656473d78597d8ff87043f</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>x</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>a4eaa9edd3f7d3923dd7d442f65b1073</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>y</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>f287581fee87f40ebf83bc2d2e47e483</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>y</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>ce909de6bda2d8c8f6eab389a5688d2c</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>operator[]</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>4d81becc177c1e29eae2362d4c9e9bf9</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>operator[]</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>3cacd08d8dd1b06acddc20e5a696c701</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>operator()</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>9397615189c20eb5b67b628f2a0f95aa</anchor>
      <arglist>(int i, int j) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>operator()</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>4ea5c0f2087e4791188ddb3a8d6e6cd9</anchor>
      <arglist>(int i, int j)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>0deafcffc5480dab2dd91fd3f6c47868</anchor>
      <arglist>(const T &amp;xval, const T &amp;yval)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>a68fca22b79d51505641f98707463be9</anchor>
      <arglist>(const R &amp;xval, const R &amp;yval)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>pop</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>538b98dd600c13401a1306ad7906535c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>begin</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>4d4f795fe655d2fa0bd3a6512cb90f7d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>begin</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>0ab7bd5490da2e3d47adcbe5c6787256</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>end</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>07750f84d56dcd4d04c5954952239304</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>end</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>d79b4843fcf39ce7809e2acf8e80ccb4</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>Map&lt; T &gt; &amp;</type>
      <name>insert</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>99c9455eef267fb54426a68a1e7e93b2</anchor>
      <arglist>(int i, const T &amp;xval, const T &amp;yval)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>insert</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>de674505204921cdb25b2ec83ee36f73</anchor>
      <arglist>(const T &amp;xval, const T &amp;yval)</arglist>
    </member>
    <member kind="function">
      <type>Map&lt; T &gt; &amp;</type>
      <name>erase</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>caf75b50fb03bf3466a59575321075c4</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1SCALARDEC</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>9e5ece076023a8a698dc86edcb1e5765</anchor>
      <arglist>(Map&lt; T &gt;, operator=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1DEC</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>e599c1f4190bb4f917e0e341c0cc2d17</anchor>
      <arglist>(Map&lt; T &gt;, operator+=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1DEC</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>eb8206b09672d0a80665d86055fc5f57</anchor>
      <arglist>(Map&lt; T &gt;, operator-=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1DEC</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>0c2eae716dda4d6647663c5c5d290c96</anchor>
      <arglist>(Map&lt; T &gt;, operator *=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1DEC</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>eea93c2f240635ee2b0f8f6e643aad70</anchor>
      <arglist>(Map&lt; T &gt;, operator/=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS1DEC</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>a0c9ab08407a9d9be30a78c37ee13283</anchor>
      <arglist>(Map&lt; T &gt;, operator%=)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS2DEC</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>87d9d9ecfe05e413af2f3e6f69d1aeb3</anchor>
      <arglist>(class TT, Map&lt; TT &gt;, operator+)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS2DEC</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>1a93b2c305627ba1062ecf470d711f2c</anchor>
      <arglist>(class TT, Map&lt; TT &gt;, operator-)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS2DEC</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>08d0efe1795a76179a3c75167cc777c9</anchor>
      <arglist>(class TT, Map&lt; TT &gt;, operator *)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS2DEC</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>f6eeeb839c9baab447bdd409762788b2</anchor>
      <arglist>(class TT, Map&lt; TT &gt;, operator/)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>CONTAINEROPS2DEC</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>24215a3f65be7ca303a2134c20a5e02e</anchor>
      <arglist>(class TT, Map&lt; TT &gt;, operator%)</arglist>
    </member>
    <member kind="function">
      <type>Map&lt; T &gt;</type>
      <name>operator-</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>041baae65e428381750c0ec658956d9b</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>Map&lt; T &gt; &amp;</type>
      <name>identity</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>7b2147cee34e252e237589eb93360f0c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>minX</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>74bc3db87223f74dd285ca9ac07ffa6a</anchor>
      <arglist>(double &amp;y, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>minY</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>c4a4b4ce183f7c5e215b0e551b2dc70d</anchor>
      <arglist>(double &amp;x, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>minXIndex</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>31d7076ada8c76ce0790be7f45879a1e</anchor>
      <arglist>(double &amp;min, double &amp;y, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>minYIndex</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>d661559c05b6ed903af19f8b8a135986</anchor>
      <arglist>(double &amp;min, double &amp;x, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>maxX</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>0218a1376dce6a3c6042554ed0d305e3</anchor>
      <arglist>(double &amp;y, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>maxY</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>be35bd21ed2ebbf77d8343ae7afad609</anchor>
      <arglist>(double &amp;x, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>maxXIndex</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>446bc69c9f8bbc59d5c376bf516be558</anchor>
      <arglist>(double &amp;max, double &amp;x, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>maxYIndex</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>9c65f0b42a91f2114f79c7328569377e</anchor>
      <arglist>(double &amp;max, double &amp;x, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxX</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>737a915d38f782a5a1a24208f4f0cf05</anchor>
      <arglist>(double &amp;min, double &amp;miny, double &amp;max, double &amp;maxy, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxY</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>5b5a4f2a78911d0a36e828fa5c095174</anchor>
      <arglist>(double &amp;min, double &amp;minx, double &amp;max, double &amp;maxx, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxXIndex</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>4d8e2adec2257890b03b43e6bdeb7bf4</anchor>
      <arglist>(double &amp;min, int &amp;minindex, double &amp;miny, double &amp;max, int &amp;maxindex, double &amp;maxy, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxYIndex</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>f9ff6bfa79bf580dcaf73a47dc31ba81</anchor>
      <arglist>(double &amp;min, int &amp;minindex, double &amp;minx, double &amp;max, int &amp;maxindex, double &amp;maxx, int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>cov</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>d5839b33d2f2dcedbc317988f212661d</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>corrCoef</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>3e05e071711c207ed4175dbe67ee1029</anchor>
      <arglist>(int first=0, int last=-1) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>propFit</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>04ec6837c74005b9f03f5927b9547ca1</anchor>
      <arglist>(int first, int last, double &amp;m, double &amp;mu, double &amp;chisq) const </arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>propFit</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>30e983e158f7cf4c6aa258d2db1472e1</anchor>
      <arglist>(double &amp;m, double &amp;mu, double &amp;chisq) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>propFit</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>117e8ae559d926034e32419b5bab3be3</anchor>
      <arglist>(int first, int last) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lineFit</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>ebd35bec34b5fec7a5e9744c4fa59631</anchor>
      <arglist>(int first, int last, double &amp;b, double &amp;bu, double &amp;m, double &amp;mu, double &amp;chisq) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lineFit</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>7006ce5169640a20eaf68ff423ed0c8d</anchor>
      <arglist>(double &amp;b, double &amp;bu, double &amp;m, double &amp;mu, double &amp;chisq) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lineFit</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>6ca1ff082b06a2dff1cf65ff3405846f</anchor>
      <arglist>(double &amp;b, double &amp;m) const</arglist>
    </member>
    <member kind="function">
      <type>ostream &amp;</type>
      <name>save</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>a43b35529dfe684f0dc45a37672bbb28</anchor>
      <arglist>(ostream &amp;str, int width=8, int prec=3, const string &amp;start=&quot;&quot;, const string &amp;separator=&quot; &quot;) const</arglist>
    </member>
    <member kind="function">
      <type>Map&lt; T &gt; &amp;</type>
      <name>save</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>dbacc9b13954cfe422d2b847410faa7e</anchor>
      <arglist>(const string &amp;file, int width=8, int prec=3) const</arglist>
    </member>
    <member kind="function">
      <type>istream &amp;</type>
      <name>load</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>80394d64a56ae105a9e5cee64d18aea3</anchor>
      <arglist>(istream &amp;str, const string &amp;stop=&quot;&quot;, string *line=0)</arglist>
    </member>
    <member kind="function">
      <type>Map&lt; T &gt; &amp;</type>
      <name>load</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>9247bc6cb37028e891d05c57a5298680</anchor>
      <arglist>(const string &amp;file, const string &amp;stop=&quot;&quot;)</arglist>
    </member>
    <member kind="friend">
      <type>friend bool</type>
      <name>operator==</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>24dd36ae33ba6cad2fb1c498e0b83f3e</anchor>
      <arglist>(const Map&lt; TT &gt; &amp;a, const Map&lt; TT &gt; &amp;b)</arglist>
    </member>
    <member kind="friend">
      <type>friend bool</type>
      <name>operator&lt;</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>09940dbbc057ed83dc6069a62021478c</anchor>
      <arglist>(const Map&lt; TT &gt; &amp;a, const Map&lt; TT &gt; &amp;b)</arglist>
    </member>
    <member kind="friend">
      <type>friend void</type>
      <name>average</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>554d1f5cf7f1bce151217e03211901ac</anchor>
      <arglist>(Map&lt; TT &gt; &amp;meantrace, const vector&lt; Map&lt; TT &gt; &gt; &amp;traces)</arglist>
    </member>
    <member kind="friend">
      <type>friend void</type>
      <name>average</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>3bf48cacadc3a9607b7479083eb22288</anchor>
      <arglist>(Map&lt; TT &gt; &amp;meantrace, Map&lt; TT &gt; &amp;stdev, const vector&lt; Map&lt; TT &gt; &gt; &amp;traces)</arglist>
    </member>
    <member kind="friend">
      <type>friend void</type>
      <name>average</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>8a87f6c95d5e7721334414eaf3d47d52</anchor>
      <arglist>(SampleData&lt; TT &gt; &amp;meantrace, const vector&lt; Map&lt; TT &gt; &gt; &amp;traces)</arglist>
    </member>
    <member kind="friend">
      <type>friend void</type>
      <name>average</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>b667e18bd0e301c74acafb283ae2e3b7</anchor>
      <arglist>(SampleData&lt; TT &gt; &amp;meantrace, SampleData&lt; TT &gt; &amp;stdev, const vector&lt; Map&lt; TT &gt; &gt; &amp;traces)</arglist>
    </member>
    <member kind="friend">
      <type>friend ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>b60beda71e80dfa71333020f3da6594b</anchor>
      <arglist>(ostream &amp;str, const Map&lt; TT &gt; &amp;a)</arglist>
    </member>
    <member kind="friend">
      <type>friend istream &amp;</type>
      <name>operator&gt;&gt;</name>
      <anchorfile>classMap.html</anchorfile>
      <anchor>d745e18a85cf69253f3673d6ce734933</anchor>
      <arglist>(istream &amp;str, Map&lt; TT &gt; &amp;a)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Polynom</name>
    <filename>classPolynom.html</filename>
    <base>BasisFunction</base>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classPolynom.html</anchorfile>
      <anchor>8a6cac8f3bbfc455fa7b1b3da6542cf3</anchor>
      <arglist>(double x, ArrayD &amp;y) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>basis</name>
      <anchorfile>classPolynom.html</anchorfile>
      <anchor>4cfd53fb5904884b4c53841cd09cb492</anchor>
      <arglist>(double x, ArrayD &amp;y) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classPolynom.html</anchorfile>
      <anchor>08eac77707e3c0ea1da4bc4d9e9fa97c</anchor>
      <arglist>(const ArrayD &amp;c, double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>value</name>
      <anchorfile>classPolynom.html</anchorfile>
      <anchor>9977d283119622038f2a6d93b2ca4426</anchor>
      <arglist>(const ArrayD &amp;c, double x) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Ran3</name>
    <filename>classRan3.html</filename>
    <base>RandomBase</base>
    <member kind="function">
      <type></type>
      <name>Ran3</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>b7379b32c1bf0dc8b0b2a6e6cb03c37c</anchor>
      <arglist>(unsigned long seed)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual unsigned long</type>
      <name>setSeed</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>ec265a1b378246a868028f1fa3621310</anchor>
      <arglist>(unsigned long seed)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual unsigned long</type>
      <name>integer</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>09d36ec7d21782f088f24b58f583d782</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual unsigned long</type>
      <name>min</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>acf80b47da17966b0889a1abbc8289cf</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual unsigned long</type>
      <name>max</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>8f400da254b1c726fba05a512f2ddd1c</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>unsigned long</type>
      <name>operator()</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>9d15565a841e557d5555e98284d052b5</anchor>
      <arglist>(unsigned long n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>62f0ca4837bb0b404360b0a735746d5e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>uniform</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>0637a4ea020f71bec4d5a59dc1abfaf1</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>gaussian</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>1f015d56cd334de841c7c99e1d19f4d8</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>exponential</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>d53f2c2e0c17a3fa0a6bbacc8c063ffe</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>gamma</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>2cc93f1decf69e7061398a7471f7f6d6</anchor>
      <arglist>(int a)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual string</type>
      <name>name</name>
      <anchorfile>classRan3.html</anchorfile>
      <anchor>745c935c2420e906ca9d6481f5aff50d</anchor>
      <arglist>(void)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RandomBase</name>
    <filename>classRandomBase.html</filename>
    <member kind="function">
      <type></type>
      <name>RandomBase</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>f82b12c089ad9a537c6041faf659189b</anchor>
      <arglist>(unsigned long seed)</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual unsigned long</type>
      <name>setSeed</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>7744a4f8622433437a953de267336034</anchor>
      <arglist>(unsigned long seed)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual unsigned long</type>
      <name>integer</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>d8a3bd663486a593b327cfceba0224b9</anchor>
      <arglist>(void)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual unsigned long</type>
      <name>min</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>e31593d12c50799375defce9ae553470</anchor>
      <arglist>(void) const=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual unsigned long</type>
      <name>max</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>200bc0389021e574f0ac9419ad801034</anchor>
      <arglist>(void) const=0</arglist>
    </member>
    <member kind="function">
      <type>unsigned long</type>
      <name>operator()</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>dd4f7473a4c12df17b492874ad1d209f</anchor>
      <arglist>(unsigned long n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>7a073d40b48dfb49c0e19392d897237c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual double</type>
      <name>uniform</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>f38ca1a4f9709d19470170c001e4b9e9</anchor>
      <arglist>(void)=0</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual double</type>
      <name>gaussian</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>76c5f35bf5cbabe580b42a069577d854</anchor>
      <arglist>(void)=0</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>exponential</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>6e866594bde5938f1d58450484778dd0</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>gamma</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>f0d085062ffac2708023cbb87df62eaf</anchor>
      <arglist>(int a)</arglist>
    </member>
    <member kind="function" virtualness="pure">
      <type>virtual string</type>
      <name>name</name>
      <anchorfile>classRandomBase.html</anchorfile>
      <anchor>d1dfc077b37c9aebb4a80b3634137a90</anchor>
      <arglist>(void)=0</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RandomStd</name>
    <filename>classRandomStd.html</filename>
    <base>RandomBase</base>
    <member kind="function">
      <type></type>
      <name>RandomStd</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>8935908f8fb23a3b1d49cd675f2588e3</anchor>
      <arglist>(unsigned long seed)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual unsigned long</type>
      <name>setSeed</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>cf0774ef534633f85d922fc55027070c</anchor>
      <arglist>(unsigned long seed)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual unsigned long</type>
      <name>integer</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>4ac5c5a7a0df06e9e149bc49e6b0474c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual unsigned long</type>
      <name>min</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>9a43e1bf73215fd51f2f87470342683e</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual unsigned long</type>
      <name>max</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>c0387f148dd5155a8d02f0f3f3392a54</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>unsigned long</type>
      <name>operator()</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>598d64985e4a7e66d53d059203635a2d</anchor>
      <arglist>(unsigned long n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>4622e833d3b905579af52534b0ac2876</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>uniform</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>1ffa6bc1d5dd5e3c55fc7a59b5649483</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>gaussian</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>623d212d27bd9a204b8757662efbad79</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>exponential</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>4bed8b9fac50f4d392e9bb333d63946b</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>gamma</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>d7b7ca94a2d53fc01a18eaf24edd8b27</anchor>
      <arglist>(int a)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual string</type>
      <name>name</name>
      <anchorfile>classRandomStd.html</anchorfile>
      <anchor>f9534aaad773b92cd0dfa9ce071a1b36</anchor>
      <arglist>(void)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>RectKernel</name>
    <filename>classRectKernel.html</filename>
    <base>Kernel</base>
    <member kind="function">
      <type></type>
      <name>RectKernel</name>
      <anchorfile>classRectKernel.html</anchorfile>
      <anchor>a7f43dd6787827c67b23e7a089c5b2b3</anchor>
      <arglist>(double stdev)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classRectKernel.html</anchorfile>
      <anchor>19c6dc480a09463bde1e9dd86a631c5b</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>value</name>
      <anchorfile>classRectKernel.html</anchorfile>
      <anchor>a0d29cbde1ed1735267c79fbd0830091</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setScale</name>
      <anchorfile>classRectKernel.html</anchorfile>
      <anchor>94c430c33c9ac8199a93044e34240aa8</anchor>
      <arglist>(double scale)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>mean</name>
      <anchorfile>classRectKernel.html</anchorfile>
      <anchor>a645e312a6528e485d0d6c8849882fad</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>stdev</name>
      <anchorfile>classRectKernel.html</anchorfile>
      <anchor>9c4c970c01c713c8888201cc8549cd5c</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setStdev</name>
      <anchorfile>classRectKernel.html</anchorfile>
      <anchor>9f4a0a1b8de4d654a1f6c54cca503a1c</anchor>
      <arglist>(double stdev)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>max</name>
      <anchorfile>classRectKernel.html</anchorfile>
      <anchor>c5cc700c73c0b3020ac3a79e8bbc9e37</anchor>
      <arglist>(void) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>SampleData</name>
    <filename>classSampleData.html</filename>
    <templarg>T</templarg>
    <base>Array</base>
    <member kind="typedef">
      <type>T</type>
      <name>value_type</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5e26f3f0544eb1e3a2c37c000aebf163</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>T *</type>
      <name>pointer</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2fe2e2a514724ff833f80195ea53c5fa</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>T &amp;</type>
      <name>reference</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>63c74e4b6507855c71a19a7a6fb0e261</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const T &amp;</type>
      <name>const_reference</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>3ee1f06cb0a15f86e6133bb4c909037e</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>int</type>
      <name>size_type</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>52f307c516d84bfaf8b9f569292c62b1</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>T *</type>
      <name>iterator</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f970e4b52e063ee35fd01826f24a0f65</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>const T *</type>
      <name>const_iterator</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fdf7d353fb3ad3c0a22229ce35bbdf29</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>double</type>
      <name>range_type</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e6db9739ef0a2c6532c0e5db0da9f5ff</anchor>
      <arglist></arglist>
    </member>
    <member kind="typedef">
      <type>LinearRange::const_iterator</type>
      <name>const_range_iterator</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9e6da37358666596818afa598db0ee52</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>4db915c2a71fa9664030a4ac8d3a724f</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fa0267df70b00aa0c090cb31ce09d6b0</anchor>
      <arglist>(int n, const T &amp;val=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>27599055896502efd3919af23687c8ea</anchor>
      <arglist>(long n, const T &amp;val=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>35d3b76a68d48569ce90820923c5ff1a</anchor>
      <arglist>(int n, double offset, double stepsize, const T &amp;val=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>cd38102eebb506f197513a47bbcae3ea</anchor>
      <arglist>(long n, double offset, double stepsize, const T &amp;val=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b5461cb3b13ccdaae067217ced2c0655</anchor>
      <arglist>(double l, double r, double stepsize, const T &amp;val=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a499a8c6a1fc2049e55e505f65ae2d0b</anchor>
      <arglist>(const LinearRange &amp;range, const T &amp;val=0)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5a5e91ff2db79886024e35ebb187f03a</anchor>
      <arglist>(const R *a, int n, double offset=0, double stepsize=1)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b4095f5a49570c84cddc016bebb3ff9a</anchor>
      <arglist>(const R *a, int n, const LinearRange &amp;range)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9ec1fda61c624a47b20edcf35d809c0c</anchor>
      <arglist>(const vector&lt; R &gt; &amp;a, double offset=0, double stepsize=1)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a8f6914116d59d4367a488ef3bf4f6ec</anchor>
      <arglist>(const vector&lt; R &gt; &amp;a, const LinearRange &amp;range)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>21bee70592ea81b2269d8a626e824bb8</anchor>
      <arglist>(const Array&lt; R &gt; &amp;a, double offset=0, double stepsize=1)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d3bdb233d9678a723e88e324d805cebd</anchor>
      <arglist>(const Array&lt; R &gt; &amp;a, const LinearRange &amp;range)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>6d15f8f62c7bad12fd395353f9816e4b</anchor>
      <arglist>(const SampleData&lt; R &gt; &amp;sa)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>81d07040ca2e9c3c5e856ac669fff3a4</anchor>
      <arglist>(const SampleData&lt; R &gt; &amp;sa, const LinearRange &amp;range)</arglist>
    </member>
    <member kind="function">
      <type></type>
      <name>SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>baa3589fc6038495184ba508eb54bfb9</anchor>
      <arglist>(const SampleData&lt; T &gt; &amp;sa)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual</type>
      <name>~SampleData</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7bec68abea097c9e5980223039294226</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1cccd37b8c75a416d36523d403e339f2</anchor>
      <arglist>(const R &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d65ec19ea894eca7d539de1a5c0aabc1</anchor>
      <arglist>(const SampleData&lt; R &gt; &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>3ab66199bb6e9285cf17408c89dc2aa2</anchor>
      <arglist>(const SampleData&lt; T &gt; &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ee227bfa4c8e0d3b4631e3f0314dffc1</anchor>
      <arglist>(const R *a, int n)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>55fbd3ddcdb64b66b187071ab78ec51e</anchor>
      <arglist>(const R *a, int n, double offset, double stepsize=1)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c9c179906ff454388d2f7afaf20ddcf9</anchor>
      <arglist>(const R &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>15a190795340be7338250c819fbd6082</anchor>
      <arglist>(const R &amp;a, double offset, double stepsize=1)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c1b43b62cccd9e2186150001851b213c</anchor>
      <arglist>(const SampleData&lt; R &gt; &amp;sa)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>54ad2bb0a10b4af725ceb3c284295a82</anchor>
      <arglist>(const SampleData&lt; T &gt; &amp;sa)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>da86688f7976e5d4111a54acf9d8ecb8</anchor>
      <arglist>(const SampleData&lt; R &gt; &amp;sa, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7199c07efa7a813ef694c37e42c91114</anchor>
      <arglist>(const SampleData&lt; R &gt; &amp;sa, double offset, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>assign</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a3fc1794635aec1bf55c61ceccd4daca</anchor>
      <arglist>(const SampleData&lt; R &gt; &amp;sa, const LinearRange &amp;range)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>whiteNoise</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>6c0725899bebab1423bbd305d1d500dc</anchor>
      <arglist>(int n, double step, double cl, double cu, R &amp;r=numerics::rnd)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>whiteNoise</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b05f0dfb6f8047958c2e7994e0762581</anchor>
      <arglist>(double l, double step, double cl, double cu, R &amp;r=numerics::rnd)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>ouNoise</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f66c5cdec3bbf253bdec831eac9a640c</anchor>
      <arglist>(int n, double step, double tau, R &amp;r=numerics::rnd)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>ouNoise</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>231af92bd9c7af4f52ad674ade073cf3</anchor>
      <arglist>(double l, double step, double tau, R &amp;r=numerics::rnd)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>6afe57ce0600c49c7762425647f6ed18</anchor>
      <arglist>(R *a, int n, const T &amp;val=0) const</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8cd723015bfbf50dd32f9128186bd5c0</anchor>
      <arglist>(R &amp;a) const</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>be17ecbda8839596d66cd1576990c0f5</anchor>
      <arglist>(SampleData&lt; R &gt; &amp;sa) const</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2b2ecf39eb8d86007192ecad570469db</anchor>
      <arglist>(double x1, double x2, SampleData&lt; R &gt; &amp;sa) const</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>20d8eff6e7d63e6bc3429fa02a6521da</anchor>
      <arglist>(double x1, double x2, Map&lt; R &gt; &amp;m) const</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>35b9c35de0e49dc3d75f0bafba424322</anchor>
      <arglist>(double x1, double x2, Array&lt; R &gt; &amp;a) const</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>copy</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8aa1b7df44bdb1568bdb0fc611ed8c5e</anchor>
      <arglist>(double x1, double x2, vector&lt; R &gt; &amp;v) const</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>afed9d1e08fbd51abdc6f9be1255f826</anchor>
      <arglist>(T a, int n=1)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c5cffb31a8eb0ac476a800c61f9f7328</anchor>
      <arglist>(const R *a, int n)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>05d7e10b1970c99887f5dbe195a32691</anchor>
      <arglist>(const R &amp;a)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>append</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>0dbc69f737414ea399504c0b3b93ab9e</anchor>
      <arglist>(const SampleData&lt; R &gt; &amp;sa)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>size</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f36c7edb18af30989b48c0a57eff6ea0</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>empty</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f15ecf5f7a17d230330cad8bc49e75df</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>resize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>43e9837386fd79427e29c4b76cc752f0</anchor>
      <arglist>(int n, const T &amp;val=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>dc8d03376f95d0cc1d6bf4c0889d3ab4</anchor>
      <arglist>(long n, const T &amp;val=0)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>89df62b6b4a9004f92f19d69398c53c6</anchor>
      <arglist>(int n, double stepsize, const T &amp;val)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fde1ba2e32470c5a42277f22f405fdc8</anchor>
      <arglist>(long n, double stepsize, const T &amp;val)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f4d3093f018c81a583c1d26739535f27</anchor>
      <arglist>(int n, double offset, double stepsize, const T &amp;val)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7ddad41fff97531eccc0205a833cb9f1</anchor>
      <arglist>(long n, double offset, double stepsize, const T &amp;val)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>6703193c3ec5cd24b0ab302477f0c666</anchor>
      <arglist>(double r, double stepsize, const T &amp;val)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>44ba02364841365a1a9b262c3e7207a3</anchor>
      <arglist>(double l, double r, double stepsize, const T &amp;val)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>resize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>74e4e224f097b080fbaa1d126047e647</anchor>
      <arglist>(const LinearRange &amp;range, const T &amp;val=0)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>clear</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>096c5fbed2f111d0fdb81d23610bbdc0</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>capacity</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>58972a1b4087d6340aa2f1a75ae6955f</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>reserve</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>177e66e8ce9d925d433df39e2a0ba2a8</anchor>
      <arglist>(int n)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>free</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c850d595979e7cdce3ecaaffbc7b1369</anchor>
      <arglist>(int n=0)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>offset</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1ea744f7e93457c97efff4ed05dfa978</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setOffset</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>6d3cc8479d38afe974bbc19ccd05e14f</anchor>
      <arglist>(double offset)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stepsize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7ea2b0f825bdf27ce8c1a2b40ce639b9</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setStepsize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e6db987f18bc19b6ee2fa6f5ea1225b3</anchor>
      <arglist>(double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scaleStepsize</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7445dafa0500ea6e16100c7f98f2cc18</anchor>
      <arglist>(double scale)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRange</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>48de9d52167d43153c44e69735f849c4</anchor>
      <arglist>(const double &amp;offset, const double &amp;stepsize)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>length</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>17a21bce7f045f0143a498b0b9cabb89</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setLength</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d69442e5b42b49451c95c18a0a51796e</anchor>
      <arglist>(double l)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rangeFront</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9ed826cd47768132517a64155ed100e7</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rangeBack</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>63cc9017ea39ce5e3864cfa88132fab5</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setRangeBack</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>be94e58d35a51424de613119f2823055</anchor>
      <arglist>(double val)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>shift</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>990796052e7b7f367ce79a52c928b9ea</anchor>
      <arglist>(double val)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>scale</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>3300ccaa453ae937b8f00d150393bcef</anchor>
      <arglist>(double scale)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>pos</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5b6f39b03b16a5efc2349ee80d040180</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>interval</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7468e5aeff4446683bfbbc93c1823c74</anchor>
      <arglist>(int indices) const </arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>index</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>39fbeeee200c43a993a73affa0ea171a</anchor>
      <arglist>(double pos) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>indices</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7b5863f931f93bb2644538a46249a931</anchor>
      <arglist>(double iv) const</arglist>
    </member>
    <member kind="function">
      <type>bool</type>
      <name>contains</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c395ac9139715bc87df13e0c7f54848e</anchor>
      <arglist>(double pos) const</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>operator[]</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1f56f7b6e8a75c7500e2e7a27b1cd441</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>operator[]</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7eaa5496fc99f033f5d59290ee7f1d15</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>at</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2a33ff720944fea622c7d6ca09e12053</anchor>
      <arglist>(int i) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>at</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>0671ac2bf32056d2d0f67b59435b524d</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>operator[]</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>74114f1fed17ca70d4707836c7a21575</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>operator[]</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>31f841fd9503fb94460201e5c72a03d4</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>operator()</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>6403d1c9e6b8f542600653a085ebc747</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>front</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d0909650e3de42c1619c6853d829c06f</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>front</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d01bb42c36357f6382c42b57c9bd5129</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const T &amp;</type>
      <name>back</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>65592867951abf62bc5dfe47b030ce2e</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>T &amp;</type>
      <name>back</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2844ee5b46c2e35459d6ca9eee91ab3e</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>push</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>bad99568af5a99b77624a6a8141ddaeb</anchor>
      <arglist>(const T &amp;val)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>pop</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>798f8d3967b32fd98e71049717c774ef</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const T *</type>
      <name>data</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f6cd123dc7f1db6c3057922e7d004f16</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>T *</type>
      <name>data</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>4c11d05511e38df45db9f5edb394c098</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const Array&lt; T &gt; &amp;</type>
      <name>array</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b17b3831df2c87f37e2dc48c49da9a05</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>Array&lt; T &gt; &amp;</type>
      <name>array</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>50ac7b6d8a9fd7bb5ddd240f1d9c7e7a</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const LinearRange &amp;</type>
      <name>range</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c01d7f449e959ffddc62fed66024dc34</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>LinearRange &amp;</type>
      <name>range</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8b43276998073506bfa2c172ef562988</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>begin</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c59ef17543d4f28795b89f69d717d0c4</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>begin</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9d8a65c25a97597a103e5b3bf445ab3f</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>end</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>742814226081ded9fd4635fc18311b53</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>const_iterator</type>
      <name>end</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>387cb135e16ed1a96bcfbcd1aff437e2</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>insert</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fee581710d2726d14f33ec4bcb06049b</anchor>
      <arglist>(int i, const T &amp;yval)</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>insert</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>3d7d31fc0e567ae98052d82bc32ec763</anchor>
      <arglist>(iterator i, const T &amp;yval)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>erase</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>4c7425b2c71e6f37639924bd37cca2ea</anchor>
      <arglist>(int i)</arglist>
    </member>
    <member kind="function">
      <type>iterator</type>
      <name>erase</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9b1a0e3f457b5713e063a269bdf1cc38</anchor>
      <arglist>(iterator i)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>05ab67425f228d184498971e83d6996e</anchor>
      <arglist>(float x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>edb3fcdd9f71895e8c016e06cbee90a5</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>debf8ceaa7cd379b7b7bc3699616f63e</anchor>
      <arglist>(long double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>6084e26452cbb1871cdb5b9d0cb2636f</anchor>
      <arglist>(signed char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>6d3e968cceda6e405200f0892c4ff19c</anchor>
      <arglist>(unsigned char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1e3154da8beba75017952c1152fac2e4</anchor>
      <arglist>(signed int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fa558efa3b3fe12c019aaebd78afde21</anchor>
      <arglist>(unsigned int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b719b2fdb017c752a92db09e0cfde36f</anchor>
      <arglist>(signed long x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>bb80f47e01d165175474331e5a1aa488</anchor>
      <arglist>(unsigned long x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator+=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>91513d04be3c2ed10b8ca632dc2c6733</anchor>
      <arglist>(const R &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator+=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>4a1817fdac20d632a4b4ca406ccf0a07</anchor>
      <arglist>(float x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator+=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>40a454a62aed0cc80dfeb8c4eea0fef8</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator+=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>298d19138fbbd9199ffe3e5515cecd7a</anchor>
      <arglist>(long double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator+=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>261ce1ae180e253321b37689b84987c6</anchor>
      <arglist>(signed char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator+=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>85f12cf674c8765229baa8a9ae26acd4</anchor>
      <arglist>(unsigned char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator+=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>3a4d36aac665e096a3aed71a8f2e6d8d</anchor>
      <arglist>(signed int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator+=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>83bb0a8b919977534127fba8236c742b</anchor>
      <arglist>(unsigned int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator+=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b98fd243e016fe3f023c815de2da7bc2</anchor>
      <arglist>(signed long x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator+=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>46404de3472b92349cd1a52622bc8471</anchor>
      <arglist>(unsigned long x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator-=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f52bd8902789bdd096b39e3e35eee2b8</anchor>
      <arglist>(const R &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator-=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a2ce0ffb5fa25e14cf9ae2a5ecd5fe37</anchor>
      <arglist>(float x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator-=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f58ef2e41a0d1d2667f0f90cd52aff3d</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator-=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fcf51106015f8b8b8f3baa9edaf6d2fc</anchor>
      <arglist>(long double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator-=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>6a48773174aeff5baeed4f8e9d13c9e9</anchor>
      <arglist>(signed char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator-=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fdd367ee1d129a11ee555a0ef7ff5a7c</anchor>
      <arglist>(unsigned char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator-=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8fe5d647d024ade7cad6cf2207cbc4e6</anchor>
      <arglist>(signed int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator-=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b7318ed1ee35752e92298a4061aaa4a9</anchor>
      <arglist>(unsigned int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator-=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>034911780ae4cf4097168c110f50d9bb</anchor>
      <arglist>(signed long x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator-=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>0add7828919f62913083caffa2aeaa13</anchor>
      <arglist>(unsigned long x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator *=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d7870e2158c525543689da48544a0690</anchor>
      <arglist>(const R &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator *=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e826f57a6dbc84d2ff8a59a3ea9ecd65</anchor>
      <arglist>(float x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator *=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8b61f58480166aea6c6d6f54e05ffb5b</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator *=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f2da2725e0619b7886dbdb2dcf667650</anchor>
      <arglist>(long double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator *=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ad20598b78c2be941760ac7ed79e4372</anchor>
      <arglist>(signed char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator *=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b8707374586fedf5a1e5fa4905c2ddbe</anchor>
      <arglist>(unsigned char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator *=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>4ec4e54b3531c9755633e35255d13b05</anchor>
      <arglist>(signed int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator *=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1c73695723e9393ca5d94990bd86e880</anchor>
      <arglist>(unsigned int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator *=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2a864084dc2e05b2776122a2c1c6e782</anchor>
      <arglist>(signed long x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator *=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ef4464daac0d1ca722ea5f34bc78f2f3</anchor>
      <arglist>(unsigned long x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator/=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>6e83c90bb601a3ed08df4946d755cea5</anchor>
      <arglist>(const R &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator/=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ec9c3e13c21695bfe53fc18ee8b37b9f</anchor>
      <arglist>(float x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator/=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>19f7b92c4bbdb3ca7fb6cdf94e6c56f9</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator/=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>32317add8718e17cfbbd97c0f4223479</anchor>
      <arglist>(long double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator/=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d2199402b0a07fcfcd89e04c2452d1dd</anchor>
      <arglist>(signed char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator/=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5b70a8ecea6b124784ccfe193b1e40a4</anchor>
      <arglist>(unsigned char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator/=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fb32759d3549a6ddb2181426d2b05ead</anchor>
      <arglist>(signed int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator/=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9f53984c8b743af83eb629db62c66767</anchor>
      <arglist>(unsigned int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator/=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f4cb6fbd44bc648d264cee5e293d50f6</anchor>
      <arglist>(signed long x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator/=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f8944b0802f27c4eab74f1ef824ab861</anchor>
      <arglist>(unsigned long x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator%=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>889cc4d0bb230d9c3e4a1906e147862c</anchor>
      <arglist>(const R &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator%=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5830136eea26f954d0662a13952afdc9</anchor>
      <arglist>(float x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator%=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7588d22e1640b19218f0215f150de20a</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator%=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>202539df70d6e0d5fa03a0c9b8b40399</anchor>
      <arglist>(long double x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator%=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1ac4c278bbf1ca9571ad4469bbc0b6e2</anchor>
      <arglist>(signed char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator%=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>477fa3e86d8ed5d571f73ffb7b0384ab</anchor>
      <arglist>(unsigned char x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator%=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fd1f4ac3438794ff72b12f840e11f37a</anchor>
      <arglist>(signed int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator%=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1f4a6285c154654293ebbf2ac66a6905</anchor>
      <arglist>(unsigned int x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator%=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>610ae8ccd89007c6048a535df10e808d</anchor>
      <arglist>(signed long x)</arglist>
    </member>
    <member kind="function">
      <type>const SampleData&lt; T &gt; &amp;</type>
      <name>operator%=</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5db1c37814b328d01f5dfff147b84089</anchor>
      <arglist>(unsigned long x)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b993932a1b4c4d67b7c17fc992056f1c</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>identity</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>022b69b2a5044b00fee48197867e356d</anchor>
      <arglist>(void)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>sin</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>0fde17e5c92f21e624c502aa2b728423</anchor>
      <arglist>(const LinearRange &amp;r, double f)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>sin</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c359ce1af43dcdb2ecc8957d3c7b9be8</anchor>
      <arglist>(int n, double offset, double stepsize, double f)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>sin</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>673cac0c55d97df61f165e5daacfd216</anchor>
      <arglist>(double l, double r, double stepsize, double f)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>cos</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b8883746a99761ba266fb538403faa9b</anchor>
      <arglist>(const LinearRange &amp;r, double f)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>cos</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8b9ca60d032ba07f158acd6727d41981</anchor>
      <arglist>(int n, double offset, double stepsize, double f)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>cos</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ede5f95e8ba42380896204e6506f2c08</anchor>
      <arglist>(double l, double r, double stepsize, double f)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>62513e437541c79d371ab50c9e7f4ad7</anchor>
      <arglist>(const LinearRange &amp;r)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>90c8ada58bb940aa5aef5e783bcce285</anchor>
      <arglist>(int n, double offset, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>70dad6642f00ab77888d06dd48bdb423</anchor>
      <arglist>(double l, double r, double stepsize)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>245a564eb1380beca3a585c5fa27dafe</anchor>
      <arglist>(const LinearRange &amp;r, double s, double m=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>61ac15261faf50d6a97864c1842763b7</anchor>
      <arglist>(int n, double offset, double stepsize, double s, double m=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d2cf83756d9ba44bf72016098f86ed3c</anchor>
      <arglist>(double l, double r, double stepsize, double s, double m=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>alpha</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8b8fccce895ffe2440093c5a20eb1ac7</anchor>
      <arglist>(const LinearRange &amp;r, double tau, double offs=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>alpha</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>aca351f7143f31a280be7392864b9223</anchor>
      <arglist>(int n, double offset, double stepsize, double tau, double offs=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>alpha</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d2c2601a90c2d41344ffa60f2e02308a</anchor>
      <arglist>(double l, double r, double stepsize, double tau, double offs=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>line</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>907aaf494886206e45899cfa60a193e2</anchor>
      <arglist>(const LinearRange &amp;r, double abscissa, double slope)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>line</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b30c135f932afae7390209a914045f88</anchor>
      <arglist>(int n, double offset, double stepsize, double abscissa, double slope)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>line</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>74ac9bd4eda2d57421805d11b660872e</anchor>
      <arglist>(double l, double r, double stepsize, double abscissa, double slope)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>rectangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>381080b8251a10673afac8a476423e7c</anchor>
      <arglist>(const LinearRange &amp;r, double period, double width, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>rectangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>27327275a7507e78c23e58f12815f297</anchor>
      <arglist>(int n, double offset, double stepsize, double period, double width, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>rectangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a274958c329e92d82c69291bd81eebb4</anchor>
      <arglist>(double l, double r, double stepsize, double period, double width, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>sawUp</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b393d52f66c3b1392828f8ba38445f58</anchor>
      <arglist>(const LinearRange &amp;r, double period, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>sawUp</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d9ec0211c101dd59e93c0907e069cca6</anchor>
      <arglist>(int n, double offset, double stepsize, double period, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>sawUp</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>137bbd3f180a8b2d2238e10f528189e3</anchor>
      <arglist>(double l, double r, double stepsize, double period, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>sawDown</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8e8ed84875d2a553ea70311532980591</anchor>
      <arglist>(const LinearRange &amp;r, double period, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>sawDown</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e2302ae85a78da232bedfeb3e1f751e7</anchor>
      <arglist>(int n, double offset, double stepsize, double period, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>sawDown</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>dfee11811dd479934e562473dac10d78</anchor>
      <arglist>(double l, double r, double stepsize, double period, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>triangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>750413c54c265b32824bf743bdeb6304</anchor>
      <arglist>(const LinearRange &amp;r, double period)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>triangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9feb98c72506280d4df2999dc100a6bb</anchor>
      <arglist>(int n, double offset, double stepsize, double period)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>triangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e7582f03635111f2589bddeab9651b1c</anchor>
      <arglist>(double l, double r, double stepsize, double period)</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>interpolate</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>59fb98cfa7736ca3047a9d407e612510</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function">
      <type>T</type>
      <name>integral</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>be3e1d4d67fc5340fe307ce53a49f6d3</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>rampUp</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>4042be37c3f89c4934042d190ca658e5</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>rampDown</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>4d45d9e3edac28aa12a74d4814392174</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>ramp</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>db488254a3d45530b13f6dc0deed3396</anchor>
      <arglist>(double x)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>addHist</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ba190bac251ed6d79377d676fd75e009</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>addHist</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>06f1e468b51d5a22659634a8cbe8eb9c</anchor>
      <arglist>(const R &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>hist</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e3719f7d89a2ffc760b13ebb341b3eef</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>hist</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b95cf0fcbb198ed27c452debd9ff97a7</anchor>
      <arglist>(const R &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>freqFilter</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d9e861e5b44a839faf35261b314a60df</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;g, bool rescale=true)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>min</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>80c6141b84ffad64b156375eb582b987</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>minIndex</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>cf7ea9358ae43c7ea4f21b9be0cbd511</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>minIndex</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>972acfe8152ffc48511bec373ab08fe8</anchor>
      <arglist>(double &amp;min, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>max</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>0019a288b87a69c0f287614ed664f5e6</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>maxIndex</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1db9bcd9319b08a3e2db035a67ca050f</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>maxIndex</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7bf98db190d429a09612303943981f38</anchor>
      <arglist>(double &amp;max, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMax</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>4a7da24a918b51f79145759095879a3f</anchor>
      <arglist>(double &amp;min, double &amp;max, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxIndex</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8b1d786850f5ece2590f6937914475d9</anchor>
      <arglist>(int &amp;minindex, int &amp;maxindex, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxIndex</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>494ba2476c876ff582aad0e5e50d9ad8</anchor>
      <arglist>(double &amp;min, int &amp;minindex, double &amp;max, int &amp;maxindex, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>mean</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7c3d9e73a7646aa0e16794abeafc5f68</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>mean</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>40171b8b3aebaca804ca7cfdf17460ef</anchor>
      <arglist>(double &amp;stdev, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>77ef57564141b5e1ced1f19e127cf42c</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c9e52a3e80606b4a2a2299d246dda5a1</anchor>
      <arglist>(double mean, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>varianceFixed</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>84ceb8c833654295060f7378071a01c9</anchor>
      <arglist>(double fixedmean, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdev</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c3851fa2bb9c95f4081d4bcd4aab12c7</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdev</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>212d10286cc15c77ca83f96226be9b71</anchor>
      <arglist>(double mean, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdevFixed</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>bb8a5b9bcd2db5ea791bf6c69fdca482</anchor>
      <arglist>(double fixedmean, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sem</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>adb82df9ccb16f6b3b77dc803d4e6eb6</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sem</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>53f1cec928fe63e70964e3c8ffed3ee3</anchor>
      <arglist>(double mean, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>semFixed</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>733ea8f3d1b70dc74cdd7f865621bc31</anchor>
      <arglist>(double fixedmean, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>absdev</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>cd79f70b011283a01daaf75f76e22203</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>absdev</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>941fe59c673b5ca269c5276e5e647fb1</anchor>
      <arglist>(double mean, double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rms</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>be9a0fdfddda5de4fd6d9b59683ce2a3</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>skewness</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b9ef738a23970a6ecd5bb08bf9c664ef</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>kurtosis</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>883ff6b4fe64f6e8143a13899bbab988</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sum</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8b1bb369d946a852979b5ed1f14c895b</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>squaredSum</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>abec42ad4780751de76409a7a028a04e</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>power</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e4bf6ac907f2a30547411d4960f8a241</anchor>
      <arglist>(double first, double last) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>troughs</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8a8d66b2e272deceb01d22f9d300bd3f</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, EventData &amp;events, double &amp;threshold, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>rising</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>63347071470a72b469e4b342a9ed6bbb</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, EventData &amp;events, double &amp;threshold, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>falling</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fc299465a9b46df7b2974a3accc12216</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, EventData &amp;events, double &amp;threshold, Check &amp;check)</arglist>
    </member>
    <member kind="function">
      <type>ostream &amp;</type>
      <name>save</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>07b719335ed3730658608bb5ff7583bf</anchor>
      <arglist>(ostream &amp;str, int width=8, int prec=3, const string &amp;start=&quot;&quot;, const string &amp;separator=&quot; &quot;) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>save</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d429a2eb036bdcb34b405f9036b50b5a</anchor>
      <arglist>(const string &amp;file, int width=8, int prec=3, const string &amp;start=&quot;&quot;, const string &amp;separator=&quot; &quot;) const</arglist>
    </member>
    <member kind="function">
      <type>istream &amp;</type>
      <name>load</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e0b11a0bbda9c4c79349e5a9f6636145</anchor>
      <arglist>(istream &amp;str, const string &amp;stop=&quot;&quot;, string *line=0)</arglist>
    </member>
    <member kind="function">
      <type>SampleData&lt; T &gt; &amp;</type>
      <name>load</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7639e5598f098ddaae30781daaa86089</anchor>
      <arglist>(const string &amp;file, const string &amp;comment=&quot;#&quot;, const string &amp;stop=&quot;&quot;)</arglist>
    </member>
    <member kind="friend">
      <type>friend bool</type>
      <name>operator==</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5403e959c69259015ada0c236022b623</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;a, const SampleData&lt; TT &gt; &amp;b)</arglist>
    </member>
    <member kind="friend">
      <type>friend bool</type>
      <name>operator&lt;</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f06f949e2ac5f1dbb94b8f9431b2e433</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;a, const SampleData&lt; TT &gt; &amp;b)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d7edc41e6e6d2024983337c2fdfb705b</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const RR &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>4e480e64da92d778a1fbb5fdad25339a</anchor>
      <arglist>(float x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>cb752c1952d3a0ad75ed2d5f2dcc173d</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, float y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a78dead0ff6b76fe7f84cf886d9cf533</anchor>
      <arglist>(double x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c34e783b0de4f5817d7e5750f80814e4</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, double y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>af983927182798fc7783b580d6e1662b</anchor>
      <arglist>(long double x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a44340bee12db3cd4cd402c54ba8d7e2</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, long double y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8dc44a1b27714cb1dff0f267421ba4b2</anchor>
      <arglist>(signed char x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c37309555081dede09d3a7f67019ff43</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed char y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ea004db0ede5871f834480850cfe4f0b</anchor>
      <arglist>(unsigned char x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ead1f0ecfc2d7d72d09fb627234d9346</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned char y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ef8c74dd3264733f7a1af98943cdf57e</anchor>
      <arglist>(signed int x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>cc0037153698a5596aaca0c742b7fd55</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed int y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2b9461a6e570a7ef8578e685bef8fd60</anchor>
      <arglist>(unsigned int x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2734c6163e076a2ed92f7f0026f5fff2</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned int y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>346786d272f8d51216c86b028cc4f199</anchor>
      <arglist>(signed long x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>65dcf9c38684f9010a08cf6b256312f0</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed long y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f4bd073f6932e851484411625799514d</anchor>
      <arglist>(unsigned long x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator+</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5c2da7d998ce4f0e4ab2e931aa1109cd</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned long y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7fbaa3182eb3ab969dc95e92ac9fd77b</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const RR &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>3b5ca492ea782ec34da5060812886f60</anchor>
      <arglist>(float x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f9b570019b50248b4f96961752ba2f5c</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, float y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5088dc422342f0e6da15aeda90390d62</anchor>
      <arglist>(double x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a6f10fa76a5a48fd56ed66e65f027078</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, double y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>0140488fab9f28a3ae4d82648a67b2a0</anchor>
      <arglist>(long double x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fcf4c397aa93b4f73424a384ec07e60d</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, long double y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2ae737fb986ddf4ac9a7cc81f6cf19c6</anchor>
      <arglist>(signed char x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c539a85d0a4a2b2aa2e65307593b80ce</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed char y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8c97c9448325d889dd5b10afd1eb2ab1</anchor>
      <arglist>(unsigned char x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>88b185a810b8b2bf83872785f2cf9164</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned char y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1b3c8b5de51b4f72718c01a6ca6056c1</anchor>
      <arglist>(signed int x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1d777d83a5e9d911777180fa1782291e</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed int y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7eea5d19d316fa4c499366b8b7b2e7de</anchor>
      <arglist>(unsigned int x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1bbdd39ce29d5aaa15f601b466eeb57e</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned int y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a0b231f8e5083eb370a1ac5e99d526bd</anchor>
      <arglist>(signed long x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a2671c2ceee7ff21511d461dcd85c326</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed long y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2a5c107447dc0dd2a70d587e622c8e54</anchor>
      <arglist>(unsigned long x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator-</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b8a6bbb52166897834b23a3f32d4ba95</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned long y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>531ad5740e397fc522aac54ab0382ac2</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const RR &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7df03cd7690da66fa50e574b809c248b</anchor>
      <arglist>(float x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b7eae7da28b986f17ea535c1c3d502fd</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, float y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9a2c325118a26cda0731161ae25701a5</anchor>
      <arglist>(double x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e87288de3f628890fb5ae39f4512e9ca</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, double y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e113e1eaaa819344af7fa76d9457941c</anchor>
      <arglist>(long double x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>0dd5182b899411789c1b6ba97ce27a2f</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, long double y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>dbd07d9d459e1112a1c63efe672691bf</anchor>
      <arglist>(signed char x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5aa408d9cf745d09dfa404a31408c561</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed char y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f661200f5c91a2445593d4944ab9cce1</anchor>
      <arglist>(unsigned char x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b9e8e7b8839be70773f33d41b96949de</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned char y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>beff5f88df8691977de7fc51f1027490</anchor>
      <arglist>(signed int x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>288d994b50cdcee688162baf8a6623dd</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed int y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>835248bed63ffda7f49ee5d61720c851</anchor>
      <arglist>(unsigned int x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>937621e292c17ad34744654479508e52</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned int y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>078277b0e0278aa88a31a5d76b36e241</anchor>
      <arglist>(signed long x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2b6e96b29b82e0652281b940c0cb8f8e</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed long y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>67dceee7d7a723ffa283415954b96335</anchor>
      <arglist>(unsigned long x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator *</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>4246a8279d1be1b1c99b600bc8978a5e</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned long y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>98d74c279358d0085af586bc03598b38</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const RR &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d4686b357e0f5f75a3e6e99749479587</anchor>
      <arglist>(float x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>fbdad5de6331bc5c70a48994bab8173d</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, float y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1af49ea86891cccc912b4db13e7655f8</anchor>
      <arglist>(double x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>43ff33cd89d7cb4f507c97d5d6ba8e7a</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, double y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5350cbea69258d50546d1e6eee7e6610</anchor>
      <arglist>(long double x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c6c38acb22931539bcd6606f66a0dab6</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, long double y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8af446d21a591c53ac376c1bde4e7a1e</anchor>
      <arglist>(signed char x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>961dad75519e14373162f0d3013ebd65</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed char y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>58609a81f89cfde1dc87db54077ff863</anchor>
      <arglist>(unsigned char x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>7046ea933ec7b85fff551ae3ff24fd43</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned char y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>cce68ba1992d82a2bab097ab91644cb3</anchor>
      <arglist>(signed int x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e4dad0996a8d918642c6e4c1d4e244b6</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed int y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>722217237677d85256ed35cb5a65eaf4</anchor>
      <arglist>(unsigned int x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b4818a90e4ea4eda61f111e05e989e19</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned int y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>30d4a8d40a0fbfd1bdf7acbb7a6728bc</anchor>
      <arglist>(signed long x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e7e02efcb00a17359854d4ab0dd7afe4</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed long y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>33a2a2445dc5a044b6f5b7770e419261</anchor>
      <arglist>(unsigned long x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator/</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>405db3c092c6fa2215c46c3b7ff845b0</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned long y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>397065aad2ef6f18c859cafa5af0e27f</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const RR &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>79f9951daddf5991f6aaa5e28a4eec46</anchor>
      <arglist>(float x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>32a579001f1aaea5e15f101b7638796d</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, float y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9b394758f88d17fbe328f4c6cfe52395</anchor>
      <arglist>(double x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>32e62fb19cda68ef23356e528f57a477</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, double y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b938f2587c15373c498e6a56e12c01a5</anchor>
      <arglist>(long double x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1b483e787a4c8796cd406d0f449939a2</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, long double y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>326f76ccbe0d03edef3c2936bedcaf5d</anchor>
      <arglist>(signed char x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>68bdd20ab8ec5fbd51e1789615a9d8a6</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed char y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>3028b19a431d934f1c54c7ea99a34e81</anchor>
      <arglist>(unsigned char x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>96b99bc71a33fefcb558209d05dd3ba1</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned char y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>cf4aa07ede8f0156502625cff9b9fc35</anchor>
      <arglist>(signed int x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a087b7bc8629c848b6224c3eed02856c</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed int y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2fa48521487add10f2bf0156e12f35aa</anchor>
      <arglist>(unsigned int x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>69865654e78c85fab7e7b161d3c28b97</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned int y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>00fa65f39e4899229432d372d0197eb0</anchor>
      <arglist>(signed long x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c075d043d0add59a9937b33b5b3aa999</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, signed long y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>958e51ed30d18cfc1ff725753bb75894</anchor>
      <arglist>(unsigned long x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>operator%</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>81b35e723a787f47fed3f07c0da77e16</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, unsigned long y)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>sin</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>2ca8a9311c117b99c3d94e29a9c3745f</anchor>
      <arglist>(const LinearRange &amp;r, double f)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>sin</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>da79cacd60374276adff6d47feafc10a</anchor>
      <arglist>(int n, double offset, double stepsize, double f)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>sin</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>03567d4c32ae856f31d49897d713d85e</anchor>
      <arglist>(double l, double r, double stepsize, double f)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>cos</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ac10de927108623ebd493d9b2fe406bf</anchor>
      <arglist>(const LinearRange &amp;r, double f)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>cos</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>93a838b4edecad4d409807c65744b95f</anchor>
      <arglist>(int n, double offset, double stepsize, double f)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>cos</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e7118c1767d44646cc9f76b749d275f6</anchor>
      <arglist>(double l, double r, double stepsize, double f)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>0505148c17d2ae38b0dae70a47f3e615</anchor>
      <arglist>(const LinearRange &amp;r)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9dece66910b922f7c4a4281f570e69f3</anchor>
      <arglist>(int n, double offset, double stepsize)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>bf455acb553d9535d1f0a581fc14bd41</anchor>
      <arglist>(double l, double r, double stepsize)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>3d3dffc468da42dcec758b59a3cb6de1</anchor>
      <arglist>(const LinearRange &amp;r, double s, double m)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9ceb94129e7b847fc649c3a129d234e1</anchor>
      <arglist>(int n, double offset, double stepsize, double s, double m)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>gauss</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ff810822b56333eef2fbdc91a9ab6132</anchor>
      <arglist>(double l, double r, double stepsize, double s, double m)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>alpha</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>76692390cbd880ffe786b9573cf1ede2</anchor>
      <arglist>(const LinearRange &amp;r, double tau, double offs)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>alpha</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1e8840e18cb0a2526d3bc6c75f7a5ab9</anchor>
      <arglist>(int n, double offset, double stepsize, double tau, double offs)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>alpha</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>a3b512d3a86968d3aa3cec81bc3f23ef</anchor>
      <arglist>(double l, double r, double stepsize, double tau, double offs)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>line</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1449cff9b650b8f02d31dc8422cf14bb</anchor>
      <arglist>(const LinearRange &amp;r, double abscissa, double slope)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>line</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d7dd86bb328c6cf3c7f7b6e5fd4690ff</anchor>
      <arglist>(int n, double offset, double stepsize, double abscissa, double slope)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>line</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5f9191851849b1ec910c0b7bbc3c9e47</anchor>
      <arglist>(double l, double r, double stepsize, double abscissa, double slope)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>rectangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>09730f8008bddf20c81e3272eb80eab4</anchor>
      <arglist>(const LinearRange &amp;r, double period, double width, double ramp)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>rectangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>1616cd9e8b2ab98c04048a64e8610642</anchor>
      <arglist>(int n, double offset, double stepsize, double period, double width, double ramp)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>rectangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>80c4eb537d7d17d804c5b5258a0a7227</anchor>
      <arglist>(double l, double r, double stepsize, double period, double width, double ramp)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>sawUp</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>e2b4cfadcea2a6584d1169f588e772c4</anchor>
      <arglist>(const LinearRange &amp;r, double period, double ramp)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>sawUp</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>c730f3179c0516eacec785a23a191031</anchor>
      <arglist>(int n, double offset, double stepsize, double period, double ramp)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>sawUp</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>25f051189d3e76bbcc3ec482d7ed8968</anchor>
      <arglist>(double l, double r, double stepsize, double period, double ramp)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>sawDown</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d0df0093cc1a4b125143471445a269fa</anchor>
      <arglist>(const LinearRange &amp;r, double period, double ramp)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>sawDown</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>934c30f4526b06ce28103ca0e1dea45f</anchor>
      <arglist>(int n, double offset, double stepsize, double period, double ramp)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>sawDown</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>9bb0bb21d750c7e3a284fff0c028fb95</anchor>
      <arglist>(double l, double r, double stepsize, double period, double ramp)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>triangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>28e109ff1e0edf9b07922df3743df11b</anchor>
      <arglist>(const LinearRange &amp;r, double period)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>triangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8b6b054953537b68bda424656369eb08</anchor>
      <arglist>(int n, double offset, double stepsize, double period)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData</type>
      <name>triangle</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>11767546b2d3747df08e7ea2eb3fdbf5</anchor>
      <arglist>(double l, double r, double stepsize, double period)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>convolve</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>ad5a46bad39c619e5957a3c2890197eb</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const RR &amp;y, int offs=0)</arglist>
    </member>
    <member kind="friend">
      <type>friend SampleData&lt; TT &gt;</type>
      <name>convolve</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>18c5715381ec4d326f6a8659172f9076</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const SampleData&lt; TT &gt; &amp;y)</arglist>
    </member>
    <member kind="friend">
      <type>friend void</type>
      <name>hcPower</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>44a248e696de451266427f1dc0c67652</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;hc, SampleData&lt; SS &gt; &amp;p)</arglist>
    </member>
    <member kind="friend">
      <type>friend void</type>
      <name>hcMagnitude</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8be4cf3639d233035733c3a640b5644b</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;hc, SampleData&lt; SS &gt; &amp;m)</arglist>
    </member>
    <member kind="friend">
      <type>friend void</type>
      <name>hcPhase</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>92582683dd15e5144f52325279ebad37</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;hc, SampleData&lt; SS &gt; &amp;p)</arglist>
    </member>
    <member kind="friend">
      <type>friend int</type>
      <name>rPSD</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8745c9728fbbdffb0c785f3c1b02d859</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, SampleData&lt; SS &gt; &amp;p, bool overlap, double(*window)(int j, int n))</arglist>
    </member>
    <member kind="friend">
      <type>friend int</type>
      <name>transfer</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>19e84bce15741af9a98dcc9435749aa5</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const SampleData&lt; SS &gt; &amp;y, SampleData&lt; RR &gt; &amp;h, bool overlap, double(*window)(int j, int n))</arglist>
    </member>
    <member kind="friend">
      <type>friend int</type>
      <name>gain</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>834bdd7dead2848fd24d9cc23d376c53</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const SampleData&lt; SS &gt; &amp;y, SampleData&lt; RR &gt; &amp;g, bool overlap, double(*window)(int j, int n))</arglist>
    </member>
    <member kind="friend">
      <type>friend int</type>
      <name>coherence</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>551dfb1f95787e39b2efb7d3c5feb18d</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const SampleData&lt; SS &gt; &amp;y, SampleData&lt; RR &gt; &amp;c, bool overlap, double(*window)(int j, int n))</arglist>
    </member>
    <member kind="friend">
      <type>friend double</type>
      <name>coherenceInfo</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>b7fea23b7239bdec004ce1ca485a0bb4</anchor>
      <arglist>(const SampleData&lt; RR &gt; &amp;c, double f0, double f1)</arglist>
    </member>
    <member kind="friend">
      <type>friend int</type>
      <name>rCSD</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d89604b9979f44b6cea671c8af93324a</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const SampleData&lt; SS &gt; &amp;y, SampleData&lt; RR &gt; &amp;c, bool overlap, double(*window)(int j, int n))</arglist>
    </member>
    <member kind="friend">
      <type>friend int</type>
      <name>spectra</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>675f981cfe76db10899caf930d0a7be1</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const SampleData&lt; SS &gt; &amp;y, SampleData&lt; RR &gt; &amp;g, SampleData&lt; RR &gt; &amp;c, SampleData&lt; RR &gt; &amp;ys, bool overlap, double(*window)(int j, int n))</arglist>
    </member>
    <member kind="friend">
      <type>friend int</type>
      <name>spectra</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>f20e19b66ede6edeeb604b5838a5f106</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, const SampleData&lt; SS &gt; &amp;y, SampleData&lt; RR &gt; &amp;g, SampleData&lt; RR &gt; &amp;c, SampleData&lt; RR &gt; &amp;cs, SampleData&lt; RR &gt; &amp;xs, SampleData&lt; RR &gt; &amp;ys, bool overlap, double(*window)(int j, int n))</arglist>
    </member>
    <member kind="friend">
      <type>friend void</type>
      <name>average</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>acadde45633c6b93c86b34a07c97e743</anchor>
      <arglist>(SampleData&lt; TT &gt; &amp;meantrace, const vector&lt; SampleData&lt; TT &gt; &gt; &amp;traces)</arglist>
    </member>
    <member kind="friend">
      <type>friend void</type>
      <name>average</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>d3ae319fab47d7fd408d7314675a1119</anchor>
      <arglist>(SampleData&lt; TT &gt; &amp;meantrace, SampleData&lt; TT &gt; &amp;stdev, const vector&lt; SampleData&lt; TT &gt; &gt; &amp;traces)</arglist>
    </member>
    <member kind="friend">
      <type>friend void</type>
      <name>peaksTroughs</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>8e4e9f90ec178f368c540aa6f2a1023f</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, EventData &amp;peaks, EventData &amp;troughs, double &amp;threshold, Check &amp;check)</arglist>
    </member>
    <member kind="friend">
      <type>friend void</type>
      <name>peaks</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>3fc8715e20fbcd7442bbaf80a81244de</anchor>
      <arglist>(const SampleData&lt; TT &gt; &amp;x, EventData &amp;events, double &amp;threshold, Check &amp;check)</arglist>
    </member>
    <member kind="friend">
      <type>friend ostream &amp;</type>
      <name>operator&lt;&lt;</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>13748bbc3df77dfdf8eec6fc705659ce</anchor>
      <arglist>(ostream &amp;str, const SampleData&lt; TT &gt; &amp;a)</arglist>
    </member>
    <member kind="friend">
      <type>friend istream &amp;</type>
      <name>operator&gt;&gt;</name>
      <anchorfile>classSampleData.html</anchorfile>
      <anchor>5830917c718775255945c1c7fb2394ea</anchor>
      <arglist>(istream &amp;str, SampleData&lt; TT &gt; &amp;a)</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>Sine</name>
    <filename>classSine.html</filename>
    <base>BasisFunction</base>
    <member kind="function">
      <type></type>
      <name>Sine</name>
      <anchorfile>classSine.html</anchorfile>
      <anchor>580097932294ac43e0d270198ba6fbeb</anchor>
      <arglist>(double freq, double phase=0.0)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>frequency</name>
      <anchorfile>classSine.html</anchorfile>
      <anchor>ef1a458129d82dc0085a4e810bf8aadc</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setFrequency</name>
      <anchorfile>classSine.html</anchorfile>
      <anchor>41ef8827edcf26757b57a88458cffd74</anchor>
      <arglist>(double freq)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>phase</name>
      <anchorfile>classSine.html</anchorfile>
      <anchor>d0db45a3fabfda78d2f397c585f52c36</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>setPhase</name>
      <anchorfile>classSine.html</anchorfile>
      <anchor>4f21ea8547835895167e862c9c0bea8d</anchor>
      <arglist>(double phase)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>operator()</name>
      <anchorfile>classSine.html</anchorfile>
      <anchor>150664dae7617a4da5a75989261a5978</anchor>
      <arglist>(double x, ArrayD &amp;y) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>basis</name>
      <anchorfile>classSine.html</anchorfile>
      <anchor>309f068eac7879b9e8f35d6b0212ba94</anchor>
      <arglist>(double x, ArrayD &amp;y) const</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classSine.html</anchorfile>
      <anchor>f8c499cfc2eaa4a31f5d99bfa3f28535</anchor>
      <arglist>(const ArrayD &amp;c, double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>value</name>
      <anchorfile>classSine.html</anchorfile>
      <anchor>4159a095b9eea7b63ebbfa8afee2a7b4</anchor>
      <arglist>(const ArrayD &amp;c, double x) const</arglist>
    </member>
  </compound>
  <compound kind="class">
    <name>TriangularKernel</name>
    <filename>classTriangularKernel.html</filename>
    <base>Kernel</base>
    <member kind="function">
      <type></type>
      <name>TriangularKernel</name>
      <anchorfile>classTriangularKernel.html</anchorfile>
      <anchor>adc5d8311bc9eaf34cbb96b11fbec61c</anchor>
      <arglist>(double stdev)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>operator()</name>
      <anchorfile>classTriangularKernel.html</anchorfile>
      <anchor>9f881f3633f170082d77700aa5e6455d</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>value</name>
      <anchorfile>classTriangularKernel.html</anchorfile>
      <anchor>bf5a13fc1833872ad6c6e25861dfc718</anchor>
      <arglist>(double x) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setScale</name>
      <anchorfile>classTriangularKernel.html</anchorfile>
      <anchor>9461379905195a8de6214c28f8e9a326</anchor>
      <arglist>(double scale)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>mean</name>
      <anchorfile>classTriangularKernel.html</anchorfile>
      <anchor>7b1988a304a0dbec636cbe8ffbff5690</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>stdev</name>
      <anchorfile>classTriangularKernel.html</anchorfile>
      <anchor>24606e4e662942197445019b12cfdfda</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual void</type>
      <name>setStdev</name>
      <anchorfile>classTriangularKernel.html</anchorfile>
      <anchor>879c487496eba6c23f6572e4c75dc9a2</anchor>
      <arglist>(double stdev)</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>max</name>
      <anchorfile>classTriangularKernel.html</anchorfile>
      <anchor>1dcd243eab6d0cf77af13f98befb1b4a</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>left</name>
      <anchorfile>classTriangularKernel.html</anchorfile>
      <anchor>41d2bf5a525073d0b7f82d8a6ea841da</anchor>
      <arglist>(void) const</arglist>
    </member>
    <member kind="function" virtualness="virtual">
      <type>virtual double</type>
      <name>right</name>
      <anchorfile>classTriangularKernel.html</anchorfile>
      <anchor>1210c44fcd039028b6616f99b5510e34</anchor>
      <arglist>(void) const</arglist>
    </member>
  </compound>
  <compound kind="namespace">
    <name>numerics</name>
    <filename>namespacenumerics.html</filename>
    <member kind="typedef">
      <type>Ran3</type>
      <name>Random</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>f79e954c21f2531c6d214d251f0ace14</anchor>
      <arglist></arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>numberFormat</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>2b7f05cd7c79f9feb2cab4eb7820cbe3</anchor>
      <arglist>(T step, T max, int &amp;width, int &amp;prec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>sin</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>cc93d7332e55b22f03af141e6640d1eb</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>cos</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>711c83c43307ed3dce0631da4fdb0f5e</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>tan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6c80af64b6701ff0ab31d60f148750d2</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>asin</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>95f52819f7c69c191edbe9aa81be2fd2</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>acos</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d5b1f9b00f6f66e717344527f79e0c22</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8df9cdd2188017337b5f1954248296ed</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5393574d59dad1ad959a926d25c4301e</anchor>
      <arglist>(const Container1 &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>sinh</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>1291f48e5f024425e36b236748fa5612</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>cosh</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e8da538f157c0a24e8855b6d47c95792</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>tanh</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>45157cc6c8d9d5ab0433ad83f010f097</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>asinh</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>94d255f3ec388f86756a50d2689e2544</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>acosh</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>500d0c5dc13c1fe431d7c27901ea8984</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>atanh</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d5ce91c829cd25fea635ecce812beddf</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>exp</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8eab5d5e54963d4accc347a623d292f0</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>log</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>417d4a47c440c36cd2b7564eb296827f</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>log10</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b965ac9d509a6f08557b02ca50c495de</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>erf</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>fce385690fa449e57b5f5fc5bd899845</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>erfc</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>77e281e22c96ab2871c10cb6c927946d</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>sqrt</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>23e12433b85f02264270a2645fa8e4ab</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>cbrt</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>82dcf3c6c22a8066af9cee54d68faff8</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7f87dc93dedc535cfb488700c6acaf38</anchor>
      <arglist>(const Container1 &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>square</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>05df7ccdc8da5b93bf7f221e45ee1082</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>cube</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6844441ef994574d213338fe048ad84d</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a3c3db11874d30cbd2c071772ca4c07f</anchor>
      <arglist>(const Container1 &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>ceil</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>596489f9cf966eb670024a6dc60b1170</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>floor</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>0fd0d6c97b0e90c1b1df4b55f0941f8f</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>abs</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4f0aea7935cd59d9807a1ff14fcf5e10</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>sin</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e2f674bc11686977fd833e49e7b03a3c</anchor>
      <arglist>(const Container &amp;vec, double f)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>cos</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>9a63e3a14bfa0ad1298b07c6c99f1f4a</anchor>
      <arglist>(const Container &amp;vec, double f)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d2ea32b06eccc0bac2fe7c6ca9ef7d72</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ba1c011a71f50700449c7edbfcea7e67</anchor>
      <arglist>(const Container1 &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ccca4d65e2d4ecdabcff1981e654e24e</anchor>
      <arglist>(const Container &amp;x, double s, double m)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>alpha</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7d8b5bdbe802519c2699104f251acefe</anchor>
      <arglist>(const Container &amp;x, double tau, double offs=0.0)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>line</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>58de3b4ef55d198ac358e924b4b3fa8f</anchor>
      <arglist>(const Container &amp;vec, double abscissa, double slope)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>rectangle</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c0915e217185c2496c2a0d1943f32877</anchor>
      <arglist>(const Container &amp;vec, double period, double width, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>sawUp</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c4cc5c3654f735a60f0a63295d177c7a</anchor>
      <arglist>(const Container &amp;vec, double period, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>sawDown</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c2ac9d9c78dd4098781447c121df18f6</anchor>
      <arglist>(const Container &amp;vec, double period, double ramp=0.0)</arglist>
    </member>
    <member kind="function">
      <type>Container</type>
      <name>triangle</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>fb56febf03d198219a12a02a3eaed409</anchor>
      <arglist>(const Container &amp;vec, double period)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>sin&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>92eff2d02a79d55afdc6a9d418b6902e</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sin&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>040a669b6abdb4ad8ceedf30ad0fc9f1</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>sin&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c75af7ff1ceb2f36a71160f6b0b8cc85</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>cos&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>87615fc8fa3bce05e8f0c49a8390ebca</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>cos&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d4de2fae7b29b1650ec747a4d5cd289c</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>cos&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8ec44110aee669e9d940413ea532b406</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>tan&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>cd310dfcc869b6bca82e79564a3a341c</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>tan&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>daead1b9472547b402a024d20e15c378</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>tan&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>0b6f076b776c26bd5341cc5406b192e7</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>asin&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b243ad1c70131ea3270a5e7740b540b4</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>asin&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8fd3c4e507c902d1443f6730be3919e3</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>asin&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>08247c04123014883b9d71883b9da67a</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>acos&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ceccf5dab6442f91dfcefe43def94929</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>acos&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a0e57d39e0dfa9662c16f90c0cf2107e</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>acos&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6d6dbd77e7b03dbe4b6c4a2c9a414965</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>atan&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>800840cbaa0eefe57c269929ce273b79</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>atan&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b43f9ab75cda027120dc108bad4f5932</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>atan&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>59c65b94254acd5f214e1d1313748620</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a05123b23bf3103d10a608a2ae36d880</anchor>
      <arglist>(const Container1 &amp;x, const float &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c1fbc9c1badb7fb75f06b8809b897ebb</anchor>
      <arglist>(const Container1 &amp;x, const double &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>bb1aecdd25e9bbe8ffbcf6be59e7d34a</anchor>
      <arglist>(const Container1 &amp;x, const long double &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5c1932197d1d214111283d4d127e610c</anchor>
      <arglist>(const float &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>cae6ccb833a8c663fd553a3b8f29cda3</anchor>
      <arglist>(const double &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6be86741cdee26f575bed05a24c3ef4c</anchor>
      <arglist>(const long double &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>f4179be360f89c0882ad751ce25a0b42</anchor>
      <arglist>(float x, float y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ee8f4ce3a773a9e6bc2b162e2a9399be</anchor>
      <arglist>(float x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>2ddde545013ec0007ebc891c7602a66a</anchor>
      <arglist>(float x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ce849e86f35d38a95471db2ffe59ee39</anchor>
      <arglist>(double x, float y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>70de9de3a2a419037d764b1aaadcc64f</anchor>
      <arglist>(double x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ced367f22e850a5efe8eccc21ebd82a3</anchor>
      <arglist>(double x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>18a562ba788f978cf3a81d0e664b0438</anchor>
      <arglist>(long double x, float y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>3689f2425b2673b9474735721af99a4d</anchor>
      <arglist>(long double x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>atan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>654f0efb838ee289b5912c0073f08077</anchor>
      <arglist>(long double x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>sinh&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>81736c998f2e637ac16cbcee0df38053</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sinh&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e6b132d7325075d42436950be5bedb3e</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>sinh&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6251a0aafb242636bb1e1c7cf649a5b3</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>cosh&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c6310e60abd014e2c60bb119213e328d</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>cosh&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e4f63af6e0ce3763f5f73b5122aaef27</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>cosh&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>474a595da8372d7d75b875bbb1b43f19</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>tanh&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>757cd99be3d8f3586c2b7f54ecab50ad</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>tanh&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>202ddcda82c806af26f64ecee3b2d67d</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>tanh&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>389263fa3cd9e9e63bcede66fb6b64d3</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>asinh&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>2c980ee71fe9f99a30e7038584ce9f34</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>asinh&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>25ed0602fa991618bf2bffd38bb32a79</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>asinh&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>cad20c4b053f2085ad212c0716c3a1eb</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>acosh&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>14df94b962bce58ae6871395010cd88f</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>acosh&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>dafc97e4a8a687449fd095ead95cf322</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>acosh&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>792d0f1321580e45b3ca7790b669505d</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>atanh&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ccb975969009309527203e22eb12e0a9</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>atanh&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>33c7a6dbdc33906fba4ac80241409c85</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>atanh&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>9204c053b0dd78ba1ed67026817420df</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>exp&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c9ed59f7fc01d64e1bd90814d9ded6ce</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>exp&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5cac54167814effdb13df6301f154832</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>exp&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>229cca533b26e6fbf8e36541f5110ff1</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>log&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c86bb5752e352661feedd5ccc87d5292</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>log&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>fcdde29514187c86666a53da992e2c4f</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>log&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>50a360234044167e3708c02bd12937b3</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>log10&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>951425af8b8e9f9ccf1ca9c8b7c6bba8</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>log10&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>9feacb993f832f6265262048bf749688</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>log10&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4db30e87c06b8380afb22e57cff51196</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>erf&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>07d5c81c33677b9a09e5046827eefca2</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>erf&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>66708ecd68850d98cb18b3df206bc8b9</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>erf&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>fcf1be4701cc40c303d4fbabe22a95a2</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>erfc&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6f897be4b761d67df91387109feb3a78</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>erfc&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>66762c52e5278399e90daaed94208243</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>erfc&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>075536d2ca7bdbed89f3d81df54d34d0</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>sqrt&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>81c3ef0f0d97a0faa4eaa4fc23086558</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sqrt&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>2cbdc38e142c047d6e4df34a67c57361</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>sqrt&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b22937fe11beddb77510181b19a04045</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>cbrt&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>21c0a1e518024611bfc3a79ac659b9ca</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>cbrt&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6f178d3de6b6bfe8e92b01fca919ec83</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>cbrt&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>96c445b7c023df1c0b07e7ff0753fb33</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d4020e4ea615b32a8cc31a9bf5463caa</anchor>
      <arglist>(const Container1 &amp;x, const float &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>f6bad233aa51f47cce13200aa5ca12af</anchor>
      <arglist>(const Container1 &amp;x, const double &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>68bdc460bbb6e1e50948142ad3e5a611</anchor>
      <arglist>(const Container1 &amp;x, const long double &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8f3ee3e3a319d7c723326ef1a93ddeb4</anchor>
      <arglist>(const float &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>231889fcbf63ce5f39501e09fd8ea0b3</anchor>
      <arglist>(const double &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>893bb98a513d07a29cadcd2cbf31be7e</anchor>
      <arglist>(const long double &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8f6f46d643b81b11f0bc33c26cf39161</anchor>
      <arglist>(float x, float y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>dd1644a0a5d50a87e77b168f00cdcdc0</anchor>
      <arglist>(float x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>93a5b8f8772d5eac5bb6c79688aee154</anchor>
      <arglist>(float x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>19db539cbe615fc6355942ddafc6f1ae</anchor>
      <arglist>(double x, float y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>16d777146e25b1a401efa7b6f803b649</anchor>
      <arglist>(double x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>edfa71c78e4575a37fb127fc0382cadb</anchor>
      <arglist>(double x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>260570212ce6439c08e9df0f0d51c4d5</anchor>
      <arglist>(long double x, float y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5459179a11af839f2fa19685af6209e5</anchor>
      <arglist>(long double x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>hypot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>befdf194fd8d3d125b90766273548786</anchor>
      <arglist>(long double x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>square&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6f47307cfe0bab810675b7d7a2d93d3c</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>square&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4f0ec0eb144d5726b055bb0101a6ecce</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>square&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7a7da5c44a8bab7666590f72bb9abd31</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>cube&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d9efd51fa965c99730415d70584433df</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>cube&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>494460270ae67fd39a7df68541748fda</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>cube&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>0bd6de069d7dd7cd8734996a92624ba7</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>63ada94be688d3e87acb829f5ec0fa59</anchor>
      <arglist>(const Container1 &amp;x, const float &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>cf3c1263ed320cfc7376145b761465ea</anchor>
      <arglist>(const Container1 &amp;x, const double &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>91ac925a9b9b0f463b09d4dc298d00cf</anchor>
      <arglist>(const Container1 &amp;x, const long double &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>993cbc03173907eee6ef047a6f1cf134</anchor>
      <arglist>(const float &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5315eb92986ead420bd36723960754fc</anchor>
      <arglist>(const double &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>91535493dcb23e065e9902c4ebbe8f68</anchor>
      <arglist>(const long double &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7176be3775e10cf6fedc18b49e197c3e</anchor>
      <arglist>(float x, float y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4670d9e1845c28e9131dda4c7063660d</anchor>
      <arglist>(float x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5a2b3a6fd16b83ba7898e9a86a697ece</anchor>
      <arglist>(float x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>840a9a499a2ec8c0e3af8c38b44b11b1</anchor>
      <arglist>(double x, float y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c4e761bbafb0f104b4b57fe8602525ef</anchor>
      <arglist>(double x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8433f61fd40dc8d7da9aa8d05c7fc27e</anchor>
      <arglist>(double x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>59950361d65cf865a1e9a7c5bd70a3f6</anchor>
      <arglist>(long double x, float y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>15aab632da91cc4190292fdd5ae7b0e7</anchor>
      <arglist>(long double x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>pow</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7b742a1959408b6835c1c1458b9c83a8</anchor>
      <arglist>(long double x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>ceil&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>1c49a441da34eaeecb559a88c9ea8bdf</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>ceil&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>9b6ded302b15abd9a5e56d18c64e2886</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>ceil&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>87130283390e49dad90d91d48f6f0eb7</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>floor&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ea344ac674b70a2b444187ca2405130f</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>floor&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>78f121902570c94108997ffbea4436f3</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>floor&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c3e751145c03abcc604181c6a84f086e</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>abs&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4ff55e7050d2612123016cfd3cc1eb2d</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>abs&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>cd701c4eb7540ff9664d083f178d06d6</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>abs&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a8de716a066c9a652e92205fe534418f</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>gauss&lt; float &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e077e766cb83ce1db7f6acbe1fbe27ea</anchor>
      <arglist>(const float &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>gauss&lt; double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d36b00bb38affa76471200e9e05f6caa</anchor>
      <arglist>(const double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>gauss&lt; long double &gt;</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>76f5b72cc1649027ca9cd12e2f7ad264</anchor>
      <arglist>(const long double &amp;x)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>2114c0a9b222ce8dd06ffefbb8888c3b</anchor>
      <arglist>(const Container1 &amp;x, const float &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6ac85675b9beaa5495ebf6615a27d5f7</anchor>
      <arglist>(const Container1 &amp;x, const double &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container1</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>062577ded63a5df14d9a35b1b3ce746c</anchor>
      <arglist>(const Container1 &amp;x, const long double &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>12207178bdf9acec9fca8ae735c6d73c</anchor>
      <arglist>(const float &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6c4b4c2827532f2bbdb0b8de8730f5c0</anchor>
      <arglist>(const double &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>Container2</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>af3a44937ba79c80ba90a866154a71a4</anchor>
      <arglist>(const long double &amp;x, const Container2 &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>float</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>33aef4dff8e2c183bfd008e18213b193</anchor>
      <arglist>(float x, float y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>30c07d44c16376e76f89441db4ca917f</anchor>
      <arglist>(float x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>54ea2212a6143af16cf2ccb639c812a3</anchor>
      <arglist>(float x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>71103b65f38cd2bbf8cffa61a9153ce5</anchor>
      <arglist>(double x, float y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>f0a21846e5ac8b393b269802a9f9b08c</anchor>
      <arglist>(double x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>381a5c41a847d655d4186041e055f3e4</anchor>
      <arglist>(double x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a8d322c93847d2e088fe57f05d125f36</anchor>
      <arglist>(long double x, float y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7f816643dc22e8dfa09bee607ad11286</anchor>
      <arglist>(long double x, double y)</arglist>
    </member>
    <member kind="function">
      <type>long double</type>
      <name>gauss</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>2e92f7f82b791ab57307fbcf4c88b571</anchor>
      <arglist>(long double x, long double y)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gaussJordan</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>227e423dd49b7d7e8028e544ff2229e5</anchor>
      <arglist>(vector&lt; ArrayD &gt; &amp;a, int n, ArrayD &amp;b)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>covarSort</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>86896b2b665fb98b6e31b224ab973c3d</anchor>
      <arglist>(vector&lt; ArrayD &gt; &amp;covar, const ArrayI &amp;paramfit, int mfit)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>expFunc</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>537d0cbd1baf829b00bd613807766f11</anchor>
      <arglist>(double x, const ArrayD &amp;p)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>expFuncDerivs</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e36465b392e297f7f3db76079399e6aa</anchor>
      <arglist>(double x, const ArrayD &amp;p, ArrayD &amp;dfdp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>expGuess</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ba6bfad77dc3b6308e29da92a48e830b</anchor>
      <arglist>(ArrayD &amp;p, double y0, double x1, double y1, double x2, double y2)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sineFunc</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>afe2cdcf98c0299110c4645750cb4555</anchor>
      <arglist>(double x, const ArrayD &amp;p)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sineFuncDerivs</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b55467a04fad4abc873815d10b90c145</anchor>
      <arglist>(double x, const ArrayD &amp;p, ArrayD &amp;dfdp)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>linearFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5916833e59f6bab2602017ad381ecfeb</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterS firsts, ForwardIterS lasts, BasisFunc &amp;funcs, ArrayD &amp;params, const ArrayI &amp;paramfit, ArrayD &amp;uncert, double &amp;chisq)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>linearFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>76f24ce102a0ad835324158fb26ed808</anchor>
      <arglist>(const ContainerX &amp;x, const ContainerY &amp;y, const ContainerS &amp;s, BasisFunc &amp;funcs, ArrayD &amp;params, const ArrayI &amp;paramfit, ArrayD &amp;uncert, double &amp;chisq)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>chisq</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d8ea2b7ed1eed126142b2c80bc5f0ab8</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterS firsts, ForwardIterS lasts, FitFunc &amp;f, ArrayD &amp;params)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>chisq</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7194eb68adac551238350fdb548c18d3</anchor>
      <arglist>(const ContainerX &amp;x, const ContainerY &amp;y, const ContainerS &amp;s, FitFunc &amp;f, ArrayD &amp;params)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>fitUncertainties</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>013695d066fb1403128823d73955d056</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterS firsts, ForwardIterS lasts, FitFunc &amp;f, const ArrayD &amp;params, const ArrayI &amp;paramfit, ArrayD &amp;uncert)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>fitUncertainties</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>99d78e80fe7e95847a45cf3d585f46d4</anchor>
      <arglist>(const ContainerX &amp;x, const ContainerY &amp;y, const ContainerS &amp;s, FitFunc &amp;f, const ArrayD &amp;params, const ArrayI &amp;paramfit, ArrayD &amp;uncert)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>simplexMin</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d9dc11ea6e1e2cc365418b47c20c1f4b</anchor>
      <arglist>(MinFunc &amp;f, ArrayD &amp;params, const ArrayI &amp;paramfit, ArrayD &amp;uncert, double &amp;chi, int *iter=NULL, ostream *os=NULL, double chieps=0.01, int maxiter=300)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>simplexFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7bf295f97b58d73d9b5bb486b2884944</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterS firsts, ForwardIterS lasts, FitFunc &amp;f, ArrayD &amp;params, const ArrayI &amp;paramfit, ArrayD &amp;uncert, double &amp;chi, int *iter=NULL, ostream *os=NULL, double chieps=0.01, int maxiter=300)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>simplexFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>51fd631d662e7e2594186a348f8eb653</anchor>
      <arglist>(const ContainerX &amp;x, const ContainerY &amp;y, const ContainerS &amp;s, FitFunc &amp;f, ArrayD &amp;params, const ArrayI &amp;paramfit, ArrayD &amp;uncert, double &amp;chisq, int *iter=NULL, ostream *os=NULL, double chieps=0.01, int maxiter=300)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>marquardtFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7bc36c59ba085d464ab554928b7aa207</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterS firsts, ForwardIterS lasts, FitFunc &amp;f, ArrayD &amp;params, const ArrayI &amp;paramfit, ArrayD &amp;uncert, double &amp;chi, int *iter=NULL, ostream *os=NULL, double chieps=0.0005, int maxiter=300)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>marquardtFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>0a551dd767f9c92349f57e7f240e5c3b</anchor>
      <arglist>(const ContainerX &amp;x, const ContainerY &amp;y, const ContainerS &amp;s, FitFunc &amp;f, ArrayD &amp;params, const ArrayI &amp;paramfit, ArrayD &amp;uncert, double &amp;chisq, int *iter=NULL, ostream *os=NULL, double chieps=0.0005, int maxiter=300)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>simplexMinTry</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>27963c119419c5490f58b45998dbf92f</anchor>
      <arglist>(vector&lt; ArrayD &gt; &amp;p, ArrayD &amp;y, ArrayD &amp;psum, int ihi, double fac, int mfit, MinFunc &amp;f, ArrayD &amp;params, const ArrayI &amp;paramfit)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>simplexFitTry</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>f4d11b4fb2c5ae9cfdf475c6c7135549</anchor>
      <arglist>(vector&lt; ArrayD &gt; &amp;p, ArrayD &amp;y, ArrayD &amp;psum, int ihi, double fac, int mfit, ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterS firsts, ForwardIterS lasts, FitFunc &amp;f, ArrayD &amp;params, const ArrayI &amp;paramfit)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>marquardtCof</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4a64c421080e62a06ab52cd5a1af79f2</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterS firsts, ForwardIterS lasts, FitFunc &amp;f, ArrayD &amp;params, const ArrayI &amp;paramfit, int mfit, double &amp;chisq, vector&lt; ArrayD &gt; &amp;alpha, ArrayD &amp;beta)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>bartlett</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4154f9275f86091777eb8dc6066a4696</anchor>
      <arglist>(int j, int n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>blackman</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>21a74c4dcc572b017a5f794ff64e24e3</anchor>
      <arglist>(int j, int n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>blackmanHarris</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>bc2f2dc6c68bd1b754361159a298dcad</anchor>
      <arglist>(int j, int n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>hamming</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>f6ff7ad48e367a6eecdcc3f4d8360a91</anchor>
      <arglist>(int j, int n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>hanning</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>39b45db0efb80cf6ac7cf843cd2cbdd4</anchor>
      <arglist>(int j, int n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>parzen</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6ffcdde2fd019a7682766292d98083f6</anchor>
      <arglist>(int j, int n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>square</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4cbded7965d88f651018d6c8e5ab56b8</anchor>
      <arglist>(int j, int n)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>welch</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>175533e459b3fbc702380559285e8d4c</anchor>
      <arglist>(int j, int n)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cFFT</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>78bf9c7126d676a49f8b683fb40a026e</anchor>
      <arglist>(RandomAccessIter first, RandomAccessIter last, int sign)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>cFFT</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5bcda1de83432dba56e36c20e58cd722</anchor>
      <arglist>(Container &amp;c, int sign)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rFFT</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>063d971def461ac373ba1c8771359506</anchor>
      <arglist>(RandomAccessIter first, RandomAccessIter last)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rFFT</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a0b13c05a3b23ec3a3ddc625108dc02a</anchor>
      <arglist>(Container &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>hcFFT</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>caa9355b65e27095cd20e7d7a5f21b3f</anchor>
      <arglist>(RandomAccessIter first, RandomAccessIter last)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>hcFFT</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>407ae9e80edbb4ef6bdf64891def3387</anchor>
      <arglist>(Container &amp;c)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hcPower</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e5fcd5624a2ec00fddd509d0f9781cc6</anchor>
      <arglist>(BidirectIterHC firsthc, BidirectIterHC lasthc, ForwardIterP firstp, ForwardIterP lastp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hcPower</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ffa16f9122095d6bd9f82c2212e5bba2</anchor>
      <arglist>(ContainerHC &amp;hc, ContainerP &amp;p)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hcMagnitude</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>949adc789109c9a580cfdbb57d16ce74</anchor>
      <arglist>(BidirectIterHC firsthc, BidirectIterHC lasthc, ForwardIterM firstm, ForwardIterM lastm)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hcMagnitude</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e6be79156cb74ee07dcdaca5e0f7fac6</anchor>
      <arglist>(ContainerHC &amp;hc, ContainerM &amp;m)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hcPhase</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>9f0e4f475d428aec9e87a2fb38ee7daf</anchor>
      <arglist>(BidirectIterHC firsthc, BidirectIterHC lasthc, ForwardIterP firstp, ForwardIterP lastp)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>hcPhase</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>9e67bf4785ff998272011b1e7b05874f</anchor>
      <arglist>(ContainerHC &amp;hc, ContainerP &amp;p)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rPSD</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>612293645485ed13cb691865cb7cf975</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterP firstp, ForwardIterP lastp, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rPSD</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b5217f89b781f94b7fb6b71cf869c12c</anchor>
      <arglist>(const ContainerX &amp;x, ContainerP &amp;p, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>transfer</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>34278727b8584956abc1e5a3cf6ab174</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, BidirectIterH firsth, BidirectIterH lasth, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>transfer</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e195420eedaa31721f41b5759786e60e</anchor>
      <arglist>(const ContainerX &amp;x, const ContainerY &amp;y, ContainerH &amp;h, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gain</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ac529d2606d71d687c8f649abdc9a5f3</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterG firstg, ForwardIterG lastg, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>gain</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>1d372bcab0426b498a0976a4ca547e96</anchor>
      <arglist>(const ContainerX &amp;x, const ContainerY &amp;y, ContainerG &amp;g, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>coherence</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e091d2b310b59d77781004fa427581f3</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterC firstc, ForwardIterC lastc, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>coherence</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a56486d36539ee135cc092860690c5b6</anchor>
      <arglist>(const ContainerX &amp;x, const ContainerY &amp;y, ContainerC &amp;c, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>coherenceInfo</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>300d51f16132f443bc14bcf9e4b539d5</anchor>
      <arglist>(ForwardIterC firstc, ForwardIterC lastc, double deltaf)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>coherenceInfo</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d6065540e1cb0c45acfe29762f92e941</anchor>
      <arglist>(ContainerC &amp;c, double deltaf)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rCSD</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>15c87dc8afccfe0f92243f25c1c70be1</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterC firstc, ForwardIterC lastc, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>rCSD</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b980f1423315404b9b018554b46a5538</anchor>
      <arglist>(const ContainerX &amp;x, const ContainerY &amp;y, ContainerC &amp;c, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>spectra</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8427c8aaf5fe2d9d382a965c726ccdf9</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterG firstg, ForwardIterG lastg, ForwardIterC firstc, ForwardIterC lastc, ForwardIterYP firstyp, ForwardIterYP lastyp, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>spectra</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>820c6ab2f3c5fa8e0a54f81f82b32651</anchor>
      <arglist>(const ContainerX &amp;x, const ContainerY &amp;y, ContainerG &amp;g, ContainerC &amp;c, ContainerYP &amp;yp, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>spectra</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6aa3de9f2e0e18d4d3a365de8fbe95ed</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterG firstg, ForwardIterG lastg, ForwardIterC firstc, ForwardIterC lastc, ForwardIterCP firstcp, ForwardIterCP lastcp, ForwardIterXP firstxp, ForwardIterXP lastxp, ForwardIterYP firstyp, ForwardIterYP lastyp, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>spectra</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>9706062186b91370158a4f38e6ede0df</anchor>
      <arglist>(const ContainerX &amp;x, const ContainerY &amp;y, ContainerG &amp;g, ContainerC &amp;c, ContainerCP &amp;cp, ContainerXP &amp;xp, ContainerYP &amp;yp, bool overlap=true, double(*window)(int j, int n)=numerics::bartlett)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>median</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>aa17ac184db32344fbdd7498a19a6001</anchor>
      <arglist>(RandomIter first, RandomIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>median</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>12d02f33caf6e0aa6bf0101394fb865e</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>quantile</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>aa24c48fb9baf382c33de463a39ee028</anchor>
      <arglist>(double f, RandomIter first, RandomIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>quantile</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>9a38aca1e43d9bfd1fecf265a72431a4</anchor>
      <arglist>(double f, const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rank</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>52b6d358ccfdd1809b0efbe6aee7cfc1</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rank</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8131db5b780a8fab13119cd9ea1d8415</anchor>
      <arglist>(Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>min</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5ae77cb3e8b97156b478c44b047a866f</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>min</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8b006fc416da6aa5d4bd5b752f68fd26</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>min</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7293bc1be8f43d90394b793dbfb2dc98</anchor>
      <arglist>(int &amp;index, ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>min</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8a2510313768a578aa48c5d71f8a41d9</anchor>
      <arglist>(int &amp;index, const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>minIndex</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>fea5fcc9afbb9e35a27417bf46b3a616</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>minIndex</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6c62b50551d3793741032a8c45ad06bb</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>max</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>36fc4a4591c414b65c865a3e856238b6</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>max</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>60c2a52350a10e8f1767b4d33d51559c</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>max</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>72f61276b593f80937e24aaabed4dcef</anchor>
      <arglist>(int &amp;index, ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>max</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>832a64f4e457685d546220cfeda8a996</anchor>
      <arglist>(int &amp;index, const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>maxIndex</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7ce54ee7730588627006f862a3c5e1f9</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>maxIndex</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>37e84544c1a0caa1636e6e5a590f785f</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMax</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d134201266e4f963027a4490bc04761c</anchor>
      <arglist>(double &amp;min, double &amp;max, ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMax</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>0af5acc22c725ba0153ebfa47c8fce3c</anchor>
      <arglist>(double &amp;min, double &amp;max, const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMax</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>57136002666b2f7e88c9667c1825bcfe</anchor>
      <arglist>(double &amp;min, int &amp;minindex, double &amp;max, int &amp;maxindex, ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMax</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>01c5df5ac3f6baa84fca968192737b90</anchor>
      <arglist>(double &amp;min, int &amp;minindex, double &amp;max, int &amp;maxindex, const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxIndex</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6fb8fec930cca2bcd278b8c3b17ebe6b</anchor>
      <arglist>(int &amp;minindex, int &amp;maxindex, ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>minMaxIndex</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b1bf54c28f9b1d31477a667b454d296f</anchor>
      <arglist>(int &amp;minindex, int &amp;maxindex, const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>minAbs</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>3008aa70b9061c6b95fef3443297aef7</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>minAbs</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>f7a6ecd8fe21eee808b922d0b8f20d0e</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>maxAbs</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7782c024a3e298c6f407c8d4cdb7c0dc</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>maxAbs</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>edf8a01812cc4dde521a55524e09639e</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>clip</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>f65547d6b4bc4967e281959342f83ae1</anchor>
      <arglist>(double min, double max, ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>int</type>
      <name>clip</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>f5bce51ddff7d89f8221ed60fba57ee4</anchor>
      <arglist>(double min, double max, Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>mean</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a7cac799bb917b83917b178e2ad55d53</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>mean</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>78d1c19f437c63df461f2e0ca30c98f8</anchor>
      <arglist>(const ContainerX &amp;vecx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wmean</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b389dc043ad3c6a18ea97175446ae95b</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterW firstw, ForwardIterW lastw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wmean</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4aea9923abcdc788fd4c4a7e487578d5</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerW &amp;vecw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>smean</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a131a7f6a17cf455c85f754c26f6205d</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterS firsts, ForwardIterS lasts)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>smean</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8c897c867f204899bf14e016228e2625</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerS &amp;vecs)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>meanStdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>1984b82211fba0e6e5d0d9897661095f</anchor>
      <arglist>(double &amp;stdev, ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>meanStdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6f17f70c0016eeb89347001c4194a76b</anchor>
      <arglist>(double &amp;stdev, const ContainerX &amp;vecx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wmeanStdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>3a5f5240d807c0c84cd9b0f2beb8f748</anchor>
      <arglist>(double &amp;stdev, ForwardIterX firstx, ForwardIterX lastx, ForwardIterW firstw, ForwardIterW lastw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wmeanStdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c7ac394e70b9170f26abb5b4d33df24a</anchor>
      <arglist>(double &amp;stdev, const ContainerX &amp;vecx, const ContainerW &amp;vecw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>smeanStdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>992d2cb5e35fb95ef2cbdbd1953d6804</anchor>
      <arglist>(double &amp;stdev, ForwardIterX firstx, ForwardIterX lastx, ForwardIterS firsts, ForwardIterS lasts)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>smeanStdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>788a7edb8d65c2bf7fb78a6e6ddbe494</anchor>
      <arglist>(double &amp;stdev, const ContainerX &amp;vecx, const ContainerS &amp;vecs)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>9b9d94a943f506b975e7232016ff47dc</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>f5b608be69ee1faa07a46c1ccea4893d</anchor>
      <arglist>(const ContainerX &amp;vecx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ce7855c85fff7dfcbc70a428c2fa4e95</anchor>
      <arglist>(double mean, ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>1211fe957a5308fc239adc4e8e090117</anchor>
      <arglist>(double mean, const ContainerX &amp;vecx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>varianceFixed</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a91f4e2add1f6ed01d2477ebb7b91c3a</anchor>
      <arglist>(double fixedmean, ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>varianceFixed</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>fb653044fcf85bcf546868ce45a9b79a</anchor>
      <arglist>(double fixedmean, const ContainerX &amp;vecx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wvariance</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>24f3c150c0d07c7e0341a255b20769b6</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterW firstw, ForwardIterW lastw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wvariance</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>88826eef981693088576348872e74ea4</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerW &amp;vecw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wvariance</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8f8e12ca8677251278c6cf5a50ab8afa</anchor>
      <arglist>(double mean, ForwardIterX firstx, ForwardIterX lastx, ForwardIterW firstw, ForwardIterW lastw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wvariance</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e6b13066f07e79c2e6c5373e3742f2a3</anchor>
      <arglist>(double mean, const ContainerX &amp;vecx, const ContainerW &amp;vecw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>69fa81d883194d3dc03194e49d7e773a</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>90fb5d72307be5a10154e76c4d3e06c4</anchor>
      <arglist>(const ContainerX &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6f56e0c70033bb8b02ad8c234c6f2f73</anchor>
      <arglist>(double mean, ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>20c6b3eefb15f065b2d4c8c1203ba98d</anchor>
      <arglist>(double mean, const ContainerX &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdevFixed</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>dca129f15bb69d336a054cc7841a4c6c</anchor>
      <arglist>(double fixedmean, ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>stdevFixed</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>cf44306606a6c380a36ab45548a71c1c</anchor>
      <arglist>(double fixedmean, const ContainerX &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wstdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d3788d9ab414111d8ca4367eb734a055</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterW firstw, ForwardIterW lastw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wstdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c04afd166eb0bb9e7f73810763cdc25f</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerW &amp;vecw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wstdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5e77af9104e4e55cb40427805abb1060</anchor>
      <arglist>(double mean, ForwardIterX firstx, ForwardIterX lastx, ForwardIterW firstw, ForwardIterW lastw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wstdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>55a34bf53c586d36aab5351f466139a8</anchor>
      <arglist>(double mean, const ContainerX &amp;vecx, const ContainerW &amp;vecw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sem</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b1a5844cd6da4b87ecd7e52cbd342179</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sem</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6a3dd6f80287d759e74e0b378cb3c6fc</anchor>
      <arglist>(const ContainerX &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sem</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d23e5e99f67da405feb13ff6285a01f2</anchor>
      <arglist>(double mean, ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sem</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>2f439b42a6f23263cbb1c1d6f22c1029</anchor>
      <arglist>(double mean, const ContainerX &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>semFixed</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>39c20244c28b035785409abe9ce22e35</anchor>
      <arglist>(double fixedmean, ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>semFixed</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>90d4698936622e372cf28602bb747135</anchor>
      <arglist>(double fixedmean, const ContainerX &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>absdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6188846aeadb6ed7a7e3dcc101f345aa</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>absdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d0c41387bd577418feba3c8a1b737957</anchor>
      <arglist>(const ContainerX &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>absdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a816b74e54afe5e5508f1a35314f15cb</anchor>
      <arglist>(double mean, ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>absdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>3140c3f07321803983f4de993755018f</anchor>
      <arglist>(double mean, const ContainerX &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wabsdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8115c5d8b5a5a2d214f9f8c32c18f881</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterW firstw, ForwardIterW lastw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wabsdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b13e2a4913ef5a04e988e2185d5c9cb9</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerW &amp;vecw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wabsdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6144df76ff8b38f971bf133d651a4c00</anchor>
      <arglist>(double mean, ForwardIterX firstx, ForwardIterX lastx, ForwardIterW firstw, ForwardIterW lastw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wabsdev</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c4b06ec1834c917fc7fd83f7d5b02ceb</anchor>
      <arglist>(double mean, const ContainerX &amp;vecx, const ContainerW &amp;vecw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rms</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>959b0e0578dbffd4442522b48f4e116a</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rms</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a4af9b30d3afa7cdbf108ef10267fe67</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>skewness</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>17c2d944995e3bc1f64539924f99ce7b</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>skewness</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>6da1667f4d002d7a93ebd2955b4cbe11</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>kurtosis</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>bfa3d4b0fedabf35624c24e495727f2c</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>kurtosis</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8d151a4cde0e8fe4fd6a436263449529</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sum</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4ac4b303cf346c0f8c63c9f5926f0213</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sum</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e5c5de0071e307a771a3559b42ceed76</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>squaredSum</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>01b4135c27e50548cf9b18e33595bc34</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>squaredSum</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b54f1632f0e41df94c056fa02408b5ef</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>magnitude</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>3d2a84314fcda37f34bc3c22dedfc8bb</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>magnitude</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>722c76290725158eedaed841a9a3acf1</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>power</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>08f625b44b289a697a382e065f3bcae0</anchor>
      <arglist>(ForwardIter first, ForwardIter last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>power</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>3bd51c04b7a11422e59f9af398943a55</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>dot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d632d982ab9bd805227007681d325823</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>dot</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>24ee1a9f0d3f2c3f1ef0583addd01062</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>average</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>e581a2463300d93c7dc62e5ec3382c0b</anchor>
      <arglist>(ContainerX &amp;x, const vector&lt; ContainerY &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>average</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>491cd69e38948348545f0c02b9a84bdc</anchor>
      <arglist>(ContainerX &amp;x, ContainerS &amp;s, const vector&lt; ContainerY &gt; &amp;y)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>cov</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c9d688692ba295b0b764fe4a11efc258</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>cov</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>af8e47946505feb803b098104dc16c71</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>corrCoef</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>0457b706c4cffc0051d3b0eea700c107</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>corrCoef</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>f8c9107cafd725ee25d79a9644167f9e</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wcorrCoef</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>10b397f33f7240451e96d2a58c6edecb</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterW firstw, ForwardIterW lastw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>wcorrCoef</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>64303eebfa52a684ee1c9b9d32e9c02a</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy, const ContainerW &amp;vecw)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>scorrCoef</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4453df4cf4a2cb7cb8e7f7debe946f50</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterS firsts, ForwardIterS lasts)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>scorrCoef</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>84261b87db34163a6185455d8912819e</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy, const ContainerS &amp;vecs)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>chisq</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a6b75050a51795d59698aada0e1d4f07</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>chisq</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>041f96e8d72e066ac4869f6fcdb15581</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>chisq</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5da49c8201ccc8e89f430589013d6c5c</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterS firsts, ForwardIterS lasts)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>chisq</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c7ced157b54e73dac0d614ed180fd680</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy, const ContainerS &amp;vecs)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>serialCorr</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d618ca11106dc24b0110f2dda0b22a16</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>serialCorr</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>2bd947eca4d1229120c9d69b7015d39f</anchor>
      <arglist>(const ContainerX &amp;vecx, ContainerY &amp;vecy)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>propFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>78f2cde731ef8d46531b7fe495b93677</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, double &amp;m, double &amp;mu, double &amp;chisq)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>propFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a8f39db1570d5f45010c22476375cf7c</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy, double &amp;m, double &amp;mu, double &amp;chisq)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>propFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>69ce7403b7a85c822765bdacc59f59a3</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterS firsts, ForwardIterS lasts, double &amp;m, double &amp;mu, double &amp;chisq)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>propFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>378d42348ab18408a60f64bda698341a</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy, const ContainerY &amp;vecs, double &amp;m, double &amp;mu, double &amp;chisq)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lineFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c606e58eaf40512d84671cad8ffa724f</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, double &amp;b, double &amp;bu, double &amp;m, double &amp;mu, double &amp;chisq)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lineFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>99e16de3db367e3a575ebeab559fcd2b</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy, double &amp;b, double &amp;bu, double &amp;m, double &amp;mu, double &amp;chisq)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lineFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>56125f9195a20eae3c9a6a79181f2b76</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx, ForwardIterY firsty, ForwardIterY lasty, ForwardIterS firsts, ForwardIterS lasts, double &amp;b, double &amp;bu, double &amp;m, double &amp;mu, double &amp;chisq)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lineFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d0f0ffb2ffbd1cfd22860ce523c2f1b2</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy, const ContainerY &amp;vecs, double &amp;b, double &amp;bu, double &amp;m, double &amp;mu, double &amp;chisq)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>detrend</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4ca14d1bd265c5c2a1c64dc1fe7527c6</anchor>
      <arglist>(ForwardIterX firstx, ForwardIterX lastx)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>detrend</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>be160e2a2689c94cab563daa8cc0f16d</anchor>
      <arglist>(ContainerX &amp;vecx)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>ca3abbb0f64f1400603b276cf05bbbef</anchor>
      <arglist>(const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>variance</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>7bc2942e557996a42cb9fd3617372f07</anchor>
      <arglist>(double mean, const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>varianceFixed</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>57d15a3089d47160ca2d3db507c32838</anchor>
      <arglist>(double fixedmean, const Container &amp;vec)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>rms</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5b75d61c816e3323d50bf4beeb682192</anchor>
      <arglist>(ForwardIterX first, ForwardIterX last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>skewness</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>a06bbeeb0e2e54ec41d5fa09b3629b9b</anchor>
      <arglist>(ForwardIterX first, ForwardIterX last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>kurtosis</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b1a074fbcb2dad96d7d88123a49f1f9a</anchor>
      <arglist>(ForwardIterX first, ForwardIterX last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>sum</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>b078ab05eb1ad646f5a7fe9daa6e5d69</anchor>
      <arglist>(ForwardIterX first, ForwardIterX last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>squaredSum</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>5873fd453325c64635d0ebe63bbddf03</anchor>
      <arglist>(ForwardIterX first, ForwardIterX last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>magnitude</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>4cc546211fa94c498cac1d764bf94dde</anchor>
      <arglist>(ForwardIterX first, ForwardIterX last)</arglist>
    </member>
    <member kind="function">
      <type>double</type>
      <name>power</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>d7f248519522bea4844847500ae4af99</anchor>
      <arglist>(ForwardIterX first, ForwardIterX last)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>propFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>0eb2c5ba7b11f453817192450587d74a</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy, const ContainerS &amp;vecs, double &amp;m, double &amp;mu, double &amp;chisq)</arglist>
    </member>
    <member kind="function">
      <type>void</type>
      <name>lineFit</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>c4e16447b4ae30c9f607dc3fffbd320c</anchor>
      <arglist>(const ContainerX &amp;vecx, const ContainerY &amp;vecy, const ContainerS &amp;vecs, double &amp;b, double &amp;bu, double &amp;m, double &amp;mu, double &amp;chisq)</arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static bool</type>
      <name>FitFlag</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>34001b995c7a70cf6dc660fb85fc8fff</anchor>
      <arglist></arglist>
    </member>
    <member kind="variable" static="yes">
      <type>static Ran3</type>
      <name>rnd</name>
      <anchorfile>namespacenumerics.html</anchorfile>
      <anchor>8e6baa849d774841207d5d7ff62a1235</anchor>
      <arglist></arglist>
    </member>
  </compound>
  <compound kind="dir">
    <name>src/</name>
    <path>/extra2/benda/prj/relacs/trunk/numerics/src/</path>
    <filename>dir_260d2913016627d6412a20c4d465545a.html</filename>
    <file>array.cc</file>
    <file>array.h</file>
    <file>basisfunction.cc</file>
    <file>basisfunction.h</file>
    <file>containerfuncs.h</file>
    <file>containerops.h</file>
    <file>cyclicarray.h</file>
    <file>cyclicsampledata.h</file>
    <file>detector.h</file>
    <file>eventdata.cc</file>
    <file>eventdata.h</file>
    <file>eventlist.cc</file>
    <file>eventlist.h</file>
    <file>fitalgorithm.cc</file>
    <file>fitalgorithm.h</file>
    <file>kernel.cc</file>
    <file>kernel.h</file>
    <file>linearrange.cc</file>
    <file>linearrange.h</file>
    <file>map.h</file>
    <file>odealgorithm.h</file>
    <file>random.cc</file>
    <file>random.h</file>
    <file>sampledata.cc</file>
    <file>sampledata.h</file>
    <file>spectrum.cc</file>
    <file>spectrum.h</file>
    <file>stats.h</file>
    <file>statstests.cc</file>
    <file>statstests.h</file>
  </compound>
</tagfile>
