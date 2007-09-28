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

<xsl:template match="/">
    <xsl:apply-templates/>
</xsl:template>

<xsl:template match="*[local-name() = 'test']">
</xsl:template>

<!-- 
	<package>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'package']">
	<xsl:message terminate="yes">package not implemented</xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'suite']">
</xsl:template>

<xsl:template match="*[local-name() = 'suite.member']">
</xsl:template>

<!-- 
	<wait>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'wait']">
	<xsl:message terminate="yes">wait not implemented</xsl:message>
</xsl:template>

<!--
================================================================
Asserts
================================================================
-->

<xsl:template match="*[local-name() = 'fail']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertNull']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertNotNull']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertTrue']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertFalse']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertEquals']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertNotEquals']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertInstanceOf']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertSize']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertDOMException']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertURIEquals']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertDOMException']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertImplementationException']">
</xsl:template>

<xsl:template match="*[local-name() = 'assertLowerSeverity']">
</xsl:template>

<!-- 
	<assertEventCount>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'assertEventCount']">
	<xsl:message terminate="yes">assertEventCount not implemented</xsl:message>
</xsl:template>

<!--
================================================================
Statements
================================================================
-->

<xsl:template match="*[local-name() = 'var']">
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
	<xsl:message terminate="yes">subtract not implemented</xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'mult']">
</xsl:template>

<!-- 
	<divide>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'divide']">
	<xsl:message terminate="yes">divide not implemented</xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'load']">
</xsl:template>

<xsl:template match="*[local-name() = 'if']">
</xsl:template>

<xsl:template match="*[local-name() = 'while']">
</xsl:template>

<xsl:template match="*[local-name() = 'try']">
</xsl:template>

<xsl:template match="*[local-name() = 'for-each']">
</xsl:template>

<xsl:template match="*[local-name() = 'comment']">
</xsl:template>

<xsl:template match="*[local-name() = 'return']">
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
	<xsl:message terminate="yes">userObj not implemented</xsl:message>
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
<xsl:template match="*[local-name() = 'same']">
	<xsl:message terminate="yes">same not implemented</xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'equals']">
</xsl:template>

<xsl:template match="*[local-name() = 'notEquals']">
</xsl:template>

<!-- 
	<lessOrEquals>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'lessOrEquals']">
	<xsl:message terminate="yes">lessOrEquals not implemented</xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'greater']">
</xsl:template>

<!-- 
	<greaterOrEquals>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'greaterOrEquals']">
	<xsl:message terminate="yes">greaterOrEquals not implemented</xsl:message>
</xsl:template>

<!-- 
	<isNull>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'isNull']">
	<xsl:message terminate="yes">isNull not implemented</xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'notNull']">
</xsl:template>

<xsl:template match="*[local-name() = 'and']">
</xsl:template>

<xsl:template match="*[local-name() = 'or']">
</xsl:template>

<!-- 
	<xor>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'xor']">
	<xsl:message terminate="yes">xor not implemented</xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'not']">
</xsl:template>

<!-- 
	<instanceOf>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'instanceOf']">
	<xsl:message terminate="yes">instanceOf not implemented</xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'isTrue']">
</xsl:template>

<xsl:template match="*[local-name() = 'isFalse']">
</xsl:template>

<!-- 
	<hasSize>
	
	Not used.
 -->
<xsl:template match="*[local-name() = 'hasSize']">
	<xsl:message terminate="yes">hasSize not implemented</xsl:message>
</xsl:template>

<xsl:template match="*[local-name() = 'contentType']">
</xsl:template>

<xsl:template match="*[local-name() = 'contains']">
</xsl:template>

<xsl:template match="*[local-name() = 'hasFeature']">
</xsl:template>

<xsl:template match="*[local-name() = 'implementationAttribute']">
</xsl:template>

</xsl:stylesheet>
