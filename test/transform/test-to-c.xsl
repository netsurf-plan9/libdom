<?xml version="1.0" encoding="ISO-8859-1"?>
<!-- 
This file is part of libdom.
Licensed under the MIT License,
               http://www.opensource.org/licenses/mit-license.php
Copyright 2007 James Shaw <jshaw@netsurf-browser.org>
-->
<!--   
This transform generates C source code from a language independent
test representation.
-->

<xsl:stylesheet version="1.0" 
    xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
    xmlns:str="http://xsltsl.org/string">
    
    	<!--
    	Import string functions from XSLT Standard Library
    	http://xsltsl.sourceforge.net/
    	-->
    	<xsl:import href="string.xsl"/>
    
    	<!--
    	The interfaces document is generated from the W3C test suite.
    	It contains the signatures of DOM interfaces, their methods
    	and attributes
    	-->
	<xsl:param name="interfaces-docname">dom1-interfaces.xml</xsl:param>
	
	<xsl:param name="target-uri-base">http://www.w3.org/2001/DOM-Test-Suite/tests/Level-1/</xsl:param>
	<xsl:output method="text" encoding="UTF-8"/>
	<xsl:variable name="domspec" select="document($interfaces-docname)"/>
	
<xsl:template match="text()" mode="body">
</xsl:template>

<!--
for anything that doesn't match another template,
we expect this the element to be found in the library located
at $interfaces-docname.

This should either be a <method> or <attribute>.  If it is neither,
we generate an <xsl:message> reporting that the element is not known, then
terminate.
-->
<xsl:template match="*" mode="body">
	<xsl:param name="vardefs"/>
	<xsl:param name="throw"/>
	<!-- the element name matched by this template -->
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
				<xsl:with-param name="vardefs" select="$vardefs"/>
				<xsl:with-param name="throw" select="$throw"/>
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
						<xsl:with-param name="vardefs" select="$vardefs"/>
						<xsl:with-param name="throw" select="$throw"/>
					</xsl:call-template>
				</xsl:when>
	
				<xsl:otherwise>
		                    <xsl:message terminate="yes"><xsl:text>Unrecognized element </xsl:text><xsl:value-of select="local-name(.)"/></xsl:message>
				</xsl:otherwise>
			</xsl:choose>
		</xsl:otherwise>
	</xsl:choose>
	<xsl:text>
	</xsl:text>
</xsl:template>

<xsl:template match="/">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*[local-name() = 'test']">
	<xsl:text>int main(int argc, char **argv)
{
</xsl:text>
	<xsl:apply-templates mode="body">
	    <xsl:with-param name="vardefs" select="*[local-name() = 'var']"/>
	    <xsl:with-param name="throw">false</xsl:with-param>
	</xsl:apply-templates>
	<xsl:text>	return 0;
}
</xsl:text>
</xsl:template>

<!-- 
	<package>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'package']">
	<xsl:message terminate="yes"><xsl:text>package not implemented</xsl:text></xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'suite']">
</xsl:template>

<xsl:template match="*[local-name() = 'suite.member']">
</xsl:template>

<!--
================================================================
Asserts
================================================================
-->

<xsl:template match="*[local-name() = 'fail']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertNull']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertNotNull']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertTrue']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertFalse']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertEquals']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertNotEquals']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertInstanceOf']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertSize']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertDOMException']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertURIEquals']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertDOMException']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertImplementationException']" mode="body">
</xsl:template>

<xsl:template match="*[local-name() = 'assertLowerSeverity']" mode="body">
</xsl:template>

<!-- 
	<assertEventCount>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'assertEventCount']">
	<xsl:message terminate="yes"><xsl:text>assertEventCount not implemented</xsl:text></xsl:message>
</xsl:template>

<!--
================================================================
Statements
================================================================
-->

<xsl:template match="*[local-name() = 'var']" mode="body">
	<xsl:value-of select="@name"/>
	<xsl:text>
</xsl:text>
</xsl:template>

<xsl:template match="*[local-name() = 'assign']">
</xsl:template>

<xsl:template match="*[local-name() = 'increment']">
</xsl:template>

<xsl:template match="*[local-name() = 'decrement']">
</xsl:template>

<xsl:template match="*[local-name() = 'append']">
</xsl:template>

<xsl:template match="*[local-name() = 'plus']">
</xsl:template>

<!-- 
	<subtract>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'subtract']">
	<xsl:message terminate="yes"><xsl:text>subtract not implemented</xsl:text></xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'mult']">
</xsl:template>

<!-- 
	<divide>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'divide']">
	<xsl:message terminate="yes"><xsl:text>divide not implemented</xsl:text></xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'load']">
</xsl:template>

<!-- 
	<if>
	
	<if>
		<condition ... />
		<statement/>
		...
		<else/> (optional)
	</if>
		
 -->
<xsl:template match="*[local-name() = 'if']" mode="body">
	<xsl:param name="vardefs"/>
	<xsl:param name="throw"/>
	<xsl:apply-templates mode="condition-before"/>
	<xsl:text>	if (</xsl:text>
		<xsl:apply-templates select="*[1]" mode="condition"/>
	<xsl:text>) {
</xsl:text>
		<!-- apply body of if -->
		<xsl:apply-templates select="*[position() &gt; 1 and local-name() != 'else']" mode="body">
			<xsl:with-param name="vardefs" select="$vardefs"/>
			<xsl:with-param name="throw" select="$throw"/>
		</xsl:apply-templates>
	<xsl:text>}</xsl:text>
	<!-- there is only ever one <else>, this is just for convenience.
	could replace it with an <xsl:if>, then do a <xsl:apply-templates select="..."> -->
	<xsl:for-each select="*[local-name()='else']">
<xsl:text> else {
	</xsl:text>
		<xsl:apply-templates mode="body">
			<xsl:with-param name="vardefs" select="$vardefs"/>
			<xsl:with-param name="throw" select="$throw"/>
	        </xsl:apply-templates>
	        <xsl:text>}</xsl:text>
	</xsl:for-each>
	<xsl:text>
	</xsl:text>
</xsl:template>

<xsl:template match="*[local-name() = 'while']">
</xsl:template>

<xsl:template match="*[local-name() = 'try']" mode="body">
	<xsl:text>	TRY
</xsl:text>
	<xsl:apply-templates select="*[local-name() != 'catch']" mode="body">
		<xsl:with-param name="vardefs" select="$vardefs"/>
		<xsl:with-param name="throw">true</xsl:with-param>
	</xsl:apply-templates>
	<xsl:text>	ENDTRY
</xsl:text>
</xsl:template>

<xsl:template match="*[local-name() = 'for-each']">
</xsl:template>

<xsl:template match="*[local-name() = 'comment']">
</xsl:template>

<xsl:template match="*[local-name() = 'return']">
</xsl:template>

<!-- 
	<wait>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'wait']" mode="body">
	<xsl:message terminate="yes"><xsl:text>wait not implemented</xsl:text></xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'substring']">
</xsl:template>

<xsl:template match="*[local-name() = 'createTempURI']">
</xsl:template>

<xsl:template match="*[local-name() = 'allErrors']">
</xsl:template>

<xsl:template match="*[local-name() = 'allNotifications']">
</xsl:template>

<xsl:template match="*[local-name() = 'operation']">
</xsl:template>

<xsl:template match="*[local-name() = 'key']">
</xsl:template>

<xsl:template match="*[local-name() = 'dst']">
</xsl:template>

<!-- 
	<userObj>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'userObj']">
	<xsl:message terminate="yes"><xsl:text>userObj not implemented</xsl:text></xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'atEvents']">
</xsl:template>

<xsl:template match="*[local-name() = 'capturedEvents']">
</xsl:template>

<xsl:template match="*[local-name() = 'bubbledEvents']">
</xsl:template>

<xsl:template match="*[local-name() = 'allEvents']">
</xsl:template>

<xsl:template match="*[local-name() = 'allEvents']">
</xsl:template>

<xsl:template match="*[local-name() = 'allEvents']">
</xsl:template>

<xsl:template match="*[local-name() = 'createXPathEvaluator']">
</xsl:template>

<xsl:template match="*[local-name() = 'getResourceURI']">
</xsl:template>

<!--
================================================================
Conditions
================================================================
-->

<!-- 
	<same>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'same']" mode="condition">
	<xsl:message terminate="yes"><xsl:text>same not implemented</xsl:text></xsl:message>
</xsl:template>

<!-- set up temporary DOMStrings and stuff here.
condition-before mode is applied before entering a statement (such as 'if') -->
<xsl:template match="*[local-name() = 'equals']" mode="condition-before">
</xsl:template>

<xsl:template match="*[local-name() = 'equals']" mode="condition">
</xsl:template>

<xsl:template match="*[local-name() = 'notEquals']" mode="condition">
</xsl:template>

<!-- 
	<lessOrEquals>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'lessOrEquals']" mode="condition">
	<xsl:message terminate="yes"><xsl:text>lessOrEquals not implemented</xsl:text></xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'greater']" mode="condition">
</xsl:template>

<!-- 
	<greaterOrEquals>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'greaterOrEquals']" mode="condition">
	<xsl:message terminate="yes"><xsl:text>greaterOrEquals not implemented</xsl:text></xsl:message>
</xsl:template>

<!-- 
	<isNull>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'isNull']" mode="condition">
	<xsl:message terminate="yes"><xsl:text>isNull not implemented</xsl:text></xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'notNull']" mode="condition">
</xsl:template>

<xsl:template match="*[local-name() = 'and']" mode="condition">
</xsl:template>

<xsl:template match="*[local-name() = 'or']" mode="condition">
    (<xsl:apply-templates select="*[1]" mode="condition"/>
    <xsl:for-each select="*[position() &gt; 1]">
        <xsl:text> | </xsl:text>
        <xsl:apply-templates select="." mode="condition"/>
    </xsl:for-each>)
</xsl:template>

<!-- 
	<xor>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'xor']" mode="condition">
	<xsl:message terminate="yes"><xsl:text>xor not implemented</xsl:text></xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'not']" mode="condition">
</xsl:template>

<!-- 
	<instanceOf>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'instanceOf']" mode="condition">
	<xsl:message terminate="yes"><xsl:text>instanceOf not implemented</xsl:text></xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'isTrue']" mode="condition">
</xsl:template>

<xsl:template match="*[local-name() = 'isFalse']" mode="condition">
</xsl:template>

<!-- 
	<hasSize>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'hasSize']" mode="condition">
	<xsl:message terminate="yes"><xsl:text>hasSize not implemented</xsl:text></xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'contentType']">
</xsl:template>

<xsl:template match="*[local-name() = 'contains']">
</xsl:template>

<xsl:template match="*[local-name() = 'hasFeature']">
</xsl:template>

<xsl:template match="*[local-name() = 'implementationAttribute']">
</xsl:template>


<!--
================================================================
Helper templates
================================================================
-->

<xsl:template name="produce-type">
    <xsl:param name="type"/>
</xsl:template>

<xsl:template name="produce-attribute">
    <xsl:param name="vardefs"/>
    <xsl:variable name="attributeName" select="local-name(.)"/>
    <xsl:choose>
        <!--  if interface is specified -->
        <xsl:when test="@interface">       
        </xsl:when>
        <xsl:otherwise>
        </xsl:otherwise>
    </xsl:choose>
    
</xsl:template>

<xsl:template name="produce-method">
    <xsl:param name="vardefs"/>
    <xsl:variable name="methodName" select="local-name(.)"/>
    <xsl:choose>
        <!--  if interface is specified -->
        <xsl:when test="@interface">
        </xsl:when>
        <xsl:otherwise>
        </xsl:otherwise>
    </xsl:choose>
</xsl:template>

</xsl:stylesheet>
