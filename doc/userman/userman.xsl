<?xml version='1.0'?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
  xmlns:fo="http://www.w3.org/1999/XSL/Format"
  version="1.0">
  <xsl:param name="id.warnings" select="1"></xsl:param>
  <xsl:param name="generate.toc">book toc,title</xsl:param>
  <xsl:param name="toc.section.depth" select="1"/>
  <xsl:param name="section.autolabel" select="1"/>
  <xsl:param name="section.autolabel.max.depth" select="2"/>
  <xsl:param name="admon.graphics" select="1"/>
  <xsl:param name="section.label.includes.component.label" select="1"/>
  <xsl:param name="html.stylesheet.type">text/css</xsl:param>
  <xsl:param name="html.stylesheet">userman.css</xsl:param>
  <xsl:param name="make.valid.html" select="1"></xsl:param>
  <xsl:param name="html.cleanup" select="1"></xsl:param>
  <xsl:param name="img.src.path"></xsl:param>
  <xsl:param name="html.ext">.html</xsl:param>
  <xsl:param name="root.filename">index</xsl:param>
  <xsl:param name="chunk.section.depth" select="0"></xsl:param>
  <xsl:param name="chunk.quietly" select="1"></xsl:param>
  <xsl:param name="use.id.as.filename" select="1"></xsl:param>
  <xsl:param name="generate.manifest" select="1"></xsl:param>
  <xsl:param name="manifest">userman.fls</xsl:param>
  <xsl:param name="navig.showtitles">1</xsl:param>
  <xsl:param name="navig.graphics" select="0"></xsl:param>
</xsl:stylesheet>


