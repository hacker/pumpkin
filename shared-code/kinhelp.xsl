<?xml version="1.0"?>
<xsl:stylesheet version="1.0"
 xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
 >
 <xsl:output
  method="text"
  encoding="us-ascii"/>
 <xsl:strip-space elements="*"/>

 <xsl:template match="winhelp">
  <xsl:text>{\rtf1\ansi</xsl:text>
  <xsl:text>&#xA;@{\footnote</xsl:text>
  <xsl:text>&#xA;THIS FILE WAS AUTOMATICALLY GENERATED FROM XML DOCUMENT.</xsl:text>
  <xsl:text>&#xA;DO NOT MODIFY THIS FILE DIRECTLY. EDIT XML DOCUMENT INSTEAD</xsl:text>
  <xsl:text>&#xA;}&#xA;</xsl:text>
  <xsl:call-template name="fonttbl"/>
  <xsl:call-template name="colortbl"/>
  <xsl:apply-templates/>
  <xsl:text>}</xsl:text>
 </xsl:template>

 <xsl:template match="topic">
  <xsl:text>&#xA;\pard\plain</xsl:text>
  <xsl:if test="descendant::*[ (@scroll='no') and
   count(preceding-sibling::*[not(@scroll='no') and (name()!='a' and @name)])=0] ">
   <xsl:text>\keepn</xsl:text>
  </xsl:if>
  <xsl:if test="@id">
   <xsl:text>&#xA;#{\footnote </xsl:text>
   <xsl:value-of select="@id"/>
   <xsl:text>}</xsl:text>
  </xsl:if>
  <xsl:if test="@title">
   <xsl:text>&#xA;${\footnote </xsl:text>
   <xsl:value-of select="@title"/>
   <xsl:text>}</xsl:text>
  </xsl:if>
  <xsl:if test="@keywords">
   <xsl:text>&#xA;K{\footnote </xsl:text>
   <xsl:value-of select="@keywords"/>
   <xsl:text>}</xsl:text>
  </xsl:if>
  <xsl:apply-templates/>
  <xsl:text>&#xA;\page&#xA;</xsl:text>
 </xsl:template>

 <xsl:template match="a[@name]">
  <xsl:text>{#{\footnote </xsl:text>
  <xsl:value-of select="@name"/>
  <xsl:text>}}</xsl:text>
 </xsl:template>
 <xsl:template match="a[@href]">
  <xsl:call-template name="hyperref">
   <xsl:with-param name="href" select="@href"/>
   <xsl:with-param name="inset"><xsl:apply-templates/></xsl:with-param>
  </xsl:call-template>
 </xsl:template>
 <xsl:template name="hyperref">
  <xsl:param name="href"/>
  <xsl:param name="inset"/>
  <xsl:choose>
   <xsl:when test="starts-with($href,'http:') or starts-with($href,'mailto:') or
    starts-with($href,'ftp:')">
    <xsl:text>{\uldb </xsl:text>
    <xsl:value-of select="$inset"/>
    <xsl:text>}{\v %!ExecFile("</xsl:text>
    <xsl:value-of select="$href"/>
    <xsl:text>")}</xsl:text>
   </xsl:when>
   <xsl:when test="starts-with($href,'#')">
    <xsl:text>{\uldb </xsl:text>
    <xsl:value-of select="$inset"/>
    <xsl:text>}{\v </xsl:text>
    <xsl:value-of select="substring($href,2)"/>
    <xsl:text>}</xsl:text>
   </xsl:when>
   <xsl:otherwise>
    <xsl:message>Warining: Unqualified hyper-reference. Using as help-internal</xsl:message>
    <xsl:text>{\uldb </xsl:text>
    <xsl:value-of select="$inset"/>
    <xsl:text>}{\v </xsl:text>
    <xsl:value-of select="$href"/>
    <xsl:text>}</xsl:text>
   </xsl:otherwise>
  </xsl:choose>
 </xsl:template>

 <xsl:template match="heading">
  <xsl:text>&#xA;</xsl:text>
  <xsl:if test="@scroll!='no'">
   <xsl:text>\pard </xsl:text>
  </xsl:if>
  <xsl:text>{ \f1\fs18\b\sb120 </xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}</xsl:text>
 </xsl:template>

 <xsl:template match="p">
  <xsl:text>&#xA;\par\sa120\sb120\qj</xsl:text>
  <xsl:if test="@scroll!='no'">
   <xsl:text>\pard</xsl:text>
  </xsl:if>
  <xsl:text> \f1\fs18\sb120 </xsl:text>
  <xsl:apply-templates/>
 </xsl:template>

 <xsl:template match="note">
  <xsl:text>&#xA;\par\sa120\sb120\qj\f1\fs16 </xsl:text>
  <xsl:apply-templates/>
 </xsl:template>

 <xsl:template match="dedication">
  <xsl:text>&#xA;\par\sa120\sb120\qr\f1\fs16 </xsl:text>
  <xsl:apply-templates/>
 </xsl:template>

 <xsl:template match="image">
  <xsl:text>\{bmct </xsl:text>
  <xsl:value-of select="@source"/>
  <xsl:text>\}</xsl:text>
 </xsl:template>

 <xsl:template match="newsfor">
  <xsl:text>&#xA;\par\pard\plain\f1\fs24\qc\cf2\b </xsl:text>
  <xsl:value-of select="@version"/>
  <xsl:text> - </xsl:text>
  <xsl:value-of select="@date"/>
  <xsl:apply-templates/>
 </xsl:template>
 <xsl:template match="ni">
  <xsl:text>&#xA;\par\pard\plain\fi0\li0\f1\fs18 \bullet  </xsl:text>
  <xsl:apply-templates/>
 </xsl:template>

 <xsl:template match="b">
  <xsl:text>{\b </xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}</xsl:text>
 </xsl:template>
 <xsl:template match="i">
  <xsl:text>{\i </xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}</xsl:text>
 </xsl:template>
 <xsl:template match="u">
  <xsl:text>{\ul </xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}</xsl:text>
 </xsl:template>
 <xsl:template match="strike">
  <xsl:text>{\strike </xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}</xsl:text>
 </xsl:template>

 <xsl:template match="kin">
  <xsl:choose>
   <xsl:when test="@href">
    <xsl:call-template name="hyperref">
     <xsl:with-param name="href" select="@href"/>
     <xsl:with-param name="inset"><xsl:text>{\b </xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text></xsl:with-param>
    </xsl:call-template>
   </xsl:when>
   <xsl:otherwise>
    <xsl:text>{\b </xsl:text>
    <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
   </xsl:otherwise>
  </xsl:choose>
 </xsl:template>
 <xsl:template match="product">
  <xsl:choose>
   <xsl:when test="@href">
    <xsl:call-template name="hyperref">
     <xsl:with-param name="href" select="@href"/>
     <xsl:with-param name="inset"><xsl:text>{\b\cf6 </xsl:text><xsl:apply-templates/><xsl:text>}</xsl:text></xsl:with-param>
    </xsl:call-template>
   </xsl:when>
   <xsl:otherwise>
    <xsl:text>{\b\cf6 </xsl:text>
    <xsl:apply-templates/>
    <xsl:text>}</xsl:text>
   </xsl:otherwise>
  </xsl:choose>
 </xsl:template>
 <xsl:template match="term">
  <xsl:text>{\i </xsl:text>
  <xsl:apply-templates/>
  <xsl:text>}</xsl:text>
 </xsl:template>
 
 <xsl:template match="rfc">
  <xsl:call-template name="hyperref">
   <xsl:with-param name="href" select="concat('http://www.rfc-editor.org/rfc/rfc',@num,'.txt')"/>
   <xsl:with-param name="inset" select="concat('{\b RFC',@num,'}')"/>
  </xsl:call-template>
 </xsl:template>

 <xsl:template match="license">
  <xsl:text>&#xA;{</xsl:text>
  <xsl:text>&#xA;\par\pard\plain\sb360\sa120 \f1\fs16 Copyright (c) </xsl:text>
  <xsl:value-of select="@years"/>
  <xsl:text> {\uldb\cf0 Klever Group (http://www.klever.net/)}{\v %!ExecFile("http://www.klever.net/")}</xsl:text>
  <xsl:text>&#xA;\par\qj\sb120\sa120</xsl:text>
  <xsl:text>Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:</xsl:text>
  <xsl:text>&#xA;\par The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.</xsl:text>
  <xsl:text>&#xA;\par \sa360 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.</xsl:text>
  <xsl:text>&#xA;}</xsl:text>
 </xsl:template>
 <xsl:template match="credits">
  <xsl:text>&#xA;\par \sa0\sb120\ql \f1\fs16 Author: {\b\uldb\cf11 Michael Krelin ({\i hacker@klever.net})}{\v %!ExecFile("mailto:hacker@klever.net")}</xsl:text>
  <xsl:text>&#xA;\par \sa0\sb0 Fan mail send to {\i\uldb gefilte@klever.net}{\v %!ExecFile("mailto:gefilte@klever.net")}</xsl:text>
 </xsl:template>

 <xsl:template match="ul">
  <xsl:text>\pard</xsl:text>
  <xsl:apply-templates/>
  <xsl:text>\pard</xsl:text>
 </xsl:template>
 <xsl:template match="li">
  <!-- TODO: could be done better, but you never know with winhelp.. -->
  <xsl:variable name="li" select=" count(ancestor::ul) "/>
  <xsl:text>&#xA;\par \fi0\li</xsl:text><xsl:value-of
   select="0"/><xsl:text> \bullet   </xsl:text>
  <xsl:apply-templates/>
 </xsl:template>
 
 <xsl:template match="topic/text()">
  <!-- maybe only omit spaces? -->
 </xsl:template>
 <xsl:template match="ul/text()">
 </xsl:template>

 <xsl:template name="colortbl">
  <xsl:text>{\colortbl;
   \red0\green0\blue0;\red0\green0\blue255;\red0\green255\blue255;\red0\green255\blue0;
   \red255\green0\blue255;\red255\green0\blue0;\red255\green255\blue0;\red255\green255\blue255;
   \red0\green0\blue128;\red0\green128\blue128;\red0\green128\blue0;\red128\green0\blue128;
   \red128\green0\blue0;\red128\green128\blue0;\red128\green128\blue128;\red192\green192\blue192;}
  </xsl:text>
 </xsl:template>
 <xsl:template name="fonttbl">
  <xsl:text>{\fonttbl</xsl:text>
  <xsl:text>{\f0\froman Times New Roman;}</xsl:text>
  <xsl:text>{\f1\fswiss Arial;}</xsl:text>
  <xsl:text>{\f3\froman Symbol;}</xsl:text>
  <xsl:text>}</xsl:text>
 </xsl:template>

</xsl:stylesheet>
