//
//  NSTextFieldClickable.m
//  Toggl Desktop on the Mac
//
//  Created by Tanel Lebedev on 11/11/2013.
//  Copyright (c) 2013 TogglDesktop developers. All rights reserved.
//

#import "NSTextFieldClickable.h"
#import "UIEvents.h"
#import "TogglDesktop-Swift.h"

@implementation NSTextFieldClickable

- (void)mouseDown:(NSEvent *)theEvent
{
	if (self.isEditable)
	{
		[[NSNotificationCenter defaultCenter] postNotificationOnMainThread:kForceCloseEditPopover
																	object:nil];
		return;
	}
	[self sendAction:@selector(textFieldClicked:) to:[self delegate]];
}

@end
