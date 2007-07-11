<?xml version="1.0" encoding="ISO-8859-1"?>
<!--   
This transform generates C source code from a language independent
test representation.
-->

<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:str="http://exslt.org/strings"><!-- TODO: exslt not currently used -->
    <!--  relative to transform   -->
    <xsl:param name="interfaces-docname">dom1-interfaces.xml</xsl:param>
    <xsl:param name="target-uri-base">http://www.w3.org/2001/DOM-Test-Suite/tests/Level-1/</xsl:param>
<xsl:output method="text" encoding="UTF-8"/>
<xsl:variable name="domspec" select="document($interfaces-docname)"/>

<!-- swallow any text which we don't understand -->
<xsl:template match="text()"/>

<!--
for anything that doesn't match another template,
we expect this the element to be found in the library located
at $interfaces-docname.

This should either be a <method> or <attribute>.  If it is neither,
we generate an <xsl:message> reporting that the element is not known.
-->
<xsl:template match="*">
	<!-- the element name matches by this template -->
	<xsl:variable name="feature" select="local-name(.)"/>
	<xsl:variable name="interface" select="@interface"/>
	
	<!--
	Try to find a method having the @name of $feature.
	If $interface is defined, make sure search for
	a match on that interface in the $domspec document.
	-->
	<xsl:variable name="method" select="$domspec/library/interface[not($interface) or @name = $interface]/method[@name = $feature]"/>
	<xsl:choose>
		<xsl:when test="$method">
			<xsl:call-template name="produce-method">
				<xsl:with-param name="method" select="$method"/>
				<!-- TODO: vardefs not yet done <xsl:with-param name="vardefs" select="$vardefs"/>-->
            		</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<!--
			Try to find an attribute having the name of $feature.
			Again, if $interface is defined, restrict the search to
			that interface
			-->
			<xsl:variable name="attribute" select="$domspec/library/interface[not($interface) or @name = $interface]/attribute[@name = $feature]"/>
			<xsl:choose>
				<xsl:when test="$attribute">
					<xsl:call-template name="produce-attribute">
						<!-- TODO: vardefs not yet done <xsl:with-param name="vardefs" select="$vardefs"/> -->
					</xsl:call-template>
				</xsl:when>
				
				<xsl:otherwise>
					<xsl:message>Unrecognized element <xsl:value-of select="local-name(.)"/></xsl:message>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:otherwise>
    </xsl:choose>
</xsl:template>

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

<!--
================================
Language construct templates
================================
-->

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
DOM templates
================================
-->

<xsl:template name="produce-method">
<!-- TODO: implement me -->
</xsl:template>

<xsl:template name="produce-attribute">
	<!-- <xsl:param name="vardefs"/> -->
	<xsl:variable name="attribName" select="local-name(.)"/>
	<xsl:choose>
		<!--  if interface is specified -->
		<xsl:when test="@interface">
			<xsl:variable name="interface" select="@interface"/>
			<xsl:call-template name="produce-specific-attribute">
				<xsl:with-param name="attribute" select="$domspec/library/interface[@name = $interface]/attribute[@name = $attribName]"/>
				<!-- <xsl:with-param name="vardefs" select="$vardefs"/> -->
			</xsl:call-template>
		</xsl:when>
		<xsl:otherwise>
			<xsl:call-template name="produce-specific-attribute">
				<xsl:with-param name="attribute" select="$domspec/library/interface/attribute[@name = $attribName]"/>
				<!-- <xsl:with-param name="vardefs" select="$vardefs"/> -->
			</xsl:call-template>
		</xsl:otherwise>
    </xsl:choose>
</xsl:template>

<xsl:template name="produce-specific-attribute">
	<!-- <xsl:param name="vardefs"/> -->
	<xsl:param name="attribute"/>
	<xsl:variable name="obj" select="@obj"/>
	<xsl:variable name="value" select="@value"/>
	<!--  check if attribute name starts with is  -->
	<xsl:if test="@value">
		<!-- TODO: set attribute to a value -->
	</xsl:if>
	<xsl:if test="@var">
		<xsl:text>
	err</xsl:text><xsl:text> = TODO_function_call(</xsl:text><xsl:value-of select="@obj"/><xsl:text>, &amp;</xsl:text><xsl:value-of select="@var"/><xsl:text>);
	assert(err == DOM_NO_ERR);
</xsl:text>

	</xsl:if>
</xsl:template>

<!--
================================
Assert templates
================================
-->

<xsl:template match="*[local-name() = 'assertNotNull']">
	<!-- TODO: what does the @id string do, and do we need it here? -->
	<xsl:text>
	assert(</xsl:text><xsl:value-of select="@actual"/><xsl:text> != NULL);
</xsl:text>
</xsl:template>

<xsl:template match="*[local-name() = 'assertNull']">
	<!-- TODO: what does the @id string do, and do we need it here? -->
	<xsl:text>
	assert(</xsl:text><xsl:value-of select="@actual"/><xsl:text> == NULL);
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
