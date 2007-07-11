<?xml version="1.0" encoding="ISO-8859-1"?>
<!--   
This transform generates C source code from a language independent
test representation.
-->

<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:str="http://exslt.org/strings"><!-- TODO: exslt not currently used -->
    <!--  relative to transform   -->
    <xsl:param name="interfaces-docname">../build/dom1-interfaces.xml</xsl:param>
    <xsl:param name="target-uri-base">http://www.w3.org/2001/DOM-Test-Suite/tests/Level-1/</xsl:param>
<xsl:output method="text" encoding="UTF-8"/>
<xsl:variable name="domspec" select="document($interfaces-docname)"/>

<!-- swallow any text which we don't understand -->
<xsl:template match="text()"/>

<xsl:template match="/">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*[local-name() = 'test']">
<xsl:text>
int main(int argc, char **argv) {
</xsl:text>
<xsl:apply-templates/>
<xsl:text>
	return 0;
}
</xsl:text>
</xsl:template>

<xsl:template match="*[local-name() = 'var']">
<xsl:text>	struct </xsl:text><xsl:call-template name="convert_var_type"> <xsl:with-param name="var_type" select="@type"/>
</xsl:call-template> *<xsl:value-of select="@name"/>;
</xsl:template>

<xsl:template match="*[local-name() = 'if']">
<xsl:text>	if (</xsl:text><!-- TODO: condition --><xsl:text>) {
</xsl:text>
<!-- TODO: statement(s) -->
<xsl:text>
	}</xsl:text>
<xsl:for-each select="*[local-name() = 'else']">
	<xsl:text> else {
	</xsl:text>
	<xsl:apply-templates/>
	<xsl:text>}</xsl:text>
</xsl:for-each>
<xsl:text>
</xsl:text>
</xsl:template>

<!--
================================
Assert templates
================================
-->

<xsl:template match="*[local-name() = 'assertNotNull']">
	<!-- TODO: what does the @id string do, and do we need it here? -->
	<xsl:text>	assert(</xsl:text><xsl:value-of select="@actual"/><xsl:text> != NULL);
</xsl:text>
</xsl:template>

<xsl:template match="*[local-name() = 'assertNull']">
	<!-- TODO: what does the @id string do, and do we need it here? -->
	<xsl:text>	assert(</xsl:text><xsl:value-of select="@actual"/><xsl:text> == NULL);
	</xsl:text>
</xsl:template>

<!-- helper templates -->

<xsl:template name="convert_var_type">
<!-- TODO: convert certain types, e.g. from DocumentType to dom_document_type -->
<xsl:param name="var_type"/>
<xsl:text>dom_</xsl:text><xsl:choose>
	<xsl:when test="$var_type = 'Document'">
		<xsl:text>document</xsl:text>
	</xsl:when>
	<xsl:when test="$var_type = 'DocumentType'">
		<xsl:text>document_type</xsl:text>
	</xsl:when>
	<xsl:when test="$var_type = 'DOMString'">
		<xsl:text>string</xsl:text>
	</xsl:when>
	<xsl:otherwise>
		<xsl:message terminate="no">Warning in convert_var_type template: unrecognised variable type '<xsl:value-of select="$var_type"/>'</xsl:message>
		<xsl:value-of select="$var_type"/>
	</xsl:otherwise>
</xsl:choose>
</xsl:template>

</xsl:stylesheet>
