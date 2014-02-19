<!DOCTYPE html>
<html>
<head>
  <meta charset="utf-8">
  <meta name="generator" content="pandoc">
  <meta name="viewport" content="width=device-width, initial-scale=1.0, user-scalable=yes">
  <title></title>
  <style type="text/css">code{white-space: pre;}</style>
  <!--[if lt IE 9]>
    <script src="http://html5shim.googlecode.com/svn/trunk/html5.js"></script>
  <![endif]-->
  <style type="text/css">
table.sourceCode, tr.sourceCode, td.lineNumbers, td.sourceCode {
  margin: 0; padding: 0; vertical-align: baseline; border: none; }
table.sourceCode { width: 100%; line-height: 100%; }
td.lineNumbers { text-align: right; padding-right: 4px; padding-left: 4px; color: #aaaaaa; border-right: 1px solid #aaaaaa; }
td.sourceCode { padding-left: 5px; }
code > span.kw { color: #007020; font-weight: bold; }
code > span.dt { color: #902000; }
code > span.dv { color: #40a070; }
code > span.bn { color: #40a070; }
code > span.fl { color: #40a070; }
code > span.ch { color: #4070a0; }
code > span.st { color: #4070a0; }
code > span.co { color: #60a0b0; font-style: italic; }
code > span.ot { color: #007020; }
code > span.al { color: #ff0000; font-weight: bold; }
code > span.fu { color: #06287e; }
code > span.er { color: #ff0000; font-weight: bold; }
  </style>
</head>
<body>
<h1 id="versos-versioned-object-storage.">Versos: VERSioned Object Storage.</h1>
<p>This library does not perform direct I/O operations into/from the objects it's managing. It's just a thin layer for grouping objects and handling versioning collectively.</p>
<p>High-level view of library:</p>
<ul>
<li>repository
<ul>
<li>coordinator
<ul>
<li>refdb
<ul>
<li>revision
<ul>
<li>revision ids</li>
<li>contents of each revision</li>
<li>object metadata (name, backend, etc.)</li>
</ul></li>
</ul></li>
<li>objectversioning
<ul>
<li>snapshot</li>
<li>remove</li>
</ul></li>
</ul></li>
</ul></li>
</ul>
<p>A repository just calls to the underlying coordinator. The coordinator uses a <code>refdb</code> to store metadata about revisions and makes sure that the integrity of the <code>refdb</code> is consistent.<sup><a href="#fn1" class="footnoteRef" id="fnref1">1</a></sup> It also makes use of the <code>snapshot()/remove()</code> functionality of provided by the <code>objectversioning</code> package (implemented through the <code>VersionedObject</code> interface) in order to make sure that object snapshots are kept in sync with the metadata.</p>
<p>The user primarly interfaces with <code>Repository</code> and <code>Version</code> classes. The former to <code>checkout()</code> existing revisions and <code>create()</code> new ones based on existing ones; the latter to <code>add()/remove()</code> objects to/from versions. For example, a user does the following:</p>
<pre class="sourceCode cpp"><code class="sourceCode cpp">MemRefDB refdb;
SingleClientCoordinator c(refdb);
Repository repo(<span class="st">&quot;mydataset&quot;</span>, c);

<span class="kw">if</span> (repo.isEmpty())
  repo.init();

Version head = repo.checkoutHEAD()

Version newVersion = repo.create(head);

VersionedObject o1, o2;

<span class="co">// initialize o1, and o2</span>

newVersion.add(o1);
newVersion.remove(o2);

newVersion.commit();</code></pre>
<p>In terms of actual I/O, the user operates on objects directly through the <code>VersionedObject</code> interface. In other words, <code>Versos</code> doesn't deal with I/O coordination, thus this is up to the application/user doing I/O. <code>Versos</code> does make sure that I/O is performed on valid versions, e.g., writes aren't done on objects that correspond to an already-committed version</p>
<p>One of the main feature of versos is its pluggability. Depending on which implementations of the tree main components of the library (<code>Coordinator</code>, <code>RefDB</code> and <code>VersionedObject</code>) are used, the consistency guarantees change.</p>
<div class="references">

</div>
<section class="footnotes">
<hr />
<ol>
<li id="fn1"><p>for example, if two users A,B checkout the same HEAD, when A <code>commit()</code>'s its revision, that revision becomes the new HEAD. What happens if user B tries to commit based on the not-HEAD-anymore revision? The coordinator should ensure that this doesn't happen.<a href="#fnref1">&#8617;</a></p></li>
</ol>
</section>
</body>
</html>
