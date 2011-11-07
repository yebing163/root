#import <stdlib.h>

#import "DetailViewController.h"
#import "RootViewController.h"

#import "DemoHelper.h"
#import "DemoBase.h"

namespace  {

enum {
   nROOTDemos = 6
};

}

@implementation RootViewController {
   NSMutableArray *tutorialNames;
   NSMutableArray *tutorialIcons;
   
   ROOT::iOS::Demos::DemoBase *demos[nROOTDemos];
}

@synthesize detailViewController;

//_________________________________________________________________
- (void)viewDidLoad
{
   NSString *filePath = [[NSBundle mainBundle] pathForResource : @"h2poly" ofType : @"root"];
   if (!ROOT::iOS::Demos::CreateTutorials(demos, [filePath cStringUsingEncoding : [NSString defaultCStringEncoding]])) {
      NSLog(@"Failed to create demos");
      exit(1);
   }

   tutorialNames = [[NSMutableArray alloc] init];
   [tutorialNames addObject:@"Hsimple"];
   [tutorialNames addObject:@"Surface"];
   [tutorialNames addObject:@"Polar graph"];
   [tutorialNames addObject:@"Lego"];
   [tutorialNames addObject:@"Exclusion graph"];
   [tutorialNames addObject:@"TH2Poly from file"];
   
   tutorialIcons = [[NSMutableArray alloc] init];
   [tutorialIcons addObject:@"hsimple_icon_n.png"];
   [tutorialIcons addObject:@"surface_icon_n.png"];
   [tutorialIcons addObject:@"polar_icon_n.png"];
   [tutorialIcons addObject:@"lego_icon_n.png"];
   [tutorialIcons addObject:@"exclusion_icon_n.png"];
   [tutorialIcons addObject:@"h2poly_icon.png"];

   //Set table view's color, row height, separator's color
   //(I want separator to be invisible).
   self.tableView.rowHeight = 72.f;
   self.tableView.backgroundColor = [UIColor lightGrayColor];
   self.tableView.separatorColor = [UIColor lightGrayColor];

   //This code was generated by ide.
   [super viewDidLoad];
   

   self.clearsSelectionOnViewWillAppear = NO;
   self.contentSizeForViewInPopover = CGSizeMake(320.0, 500.0);//Default height was 600., I've changed it to 500.
}

//_________________________________________________________________
- (void) viewWillAppear : (BOOL)animated
{
   [super viewWillAppear : animated];
}

//_________________________________________________________________
- (void) viewDidAppear : (BOOL)animated
{
   [super viewDidAppear : animated];
}

//_________________________________________________________________
- (void) viewWillDisappear : (BOOL)animated
{
	[super viewWillDisappear : animated];
}

//_________________________________________________________________
- (void) viewDidDisappear : (BOOL)animated
{
	[super viewDidDisappear : animated];
}

//_________________________________________________________________
- (BOOL) shouldAutorotateToInterfaceOrientation : (UIInterfaceOrientation)interfaceOrientation
{
   return YES;
}

//_________________________________________________________________
- (NSInteger) numberOfSectionsInTableView : (UITableView *)tableView
{
   return 1;
   		
}

//_________________________________________________________________
- (NSInteger) tableView : (UITableView *)tableView numberOfRowsInSection : (NSInteger)section
{
   return [tutorialNames count];   		
}

//_________________________________________________________________
- (UITableViewCell *) tableView : (UITableView *)tableView cellForRowAtIndexPath : (NSIndexPath *)indexPath
{
   static NSString *CellIdentifier = @"Cell";
   
   UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier : CellIdentifier];
   if (cell == nil)
      cell = [[UITableViewCell alloc] initWithStyle : UITableViewCellStyleDefault reuseIdentifier : CellIdentifier];

   // Configure the cell.
   const BOOL useDarkBackground = indexPath.row % 2;
   NSString *backgroundImagePath = [[NSBundle mainBundle] pathForResource : useDarkBackground ? @"DarkBackground" : @"LightBackground" ofType : @"png"];

   UIImage *backgroundImage = [[UIImage imageWithContentsOfFile : backgroundImagePath] stretchableImageWithLeftCapWidth : 0.f topCapHeight : 1.f];   
   cell.backgroundView = [[UIImageView alloc] initWithImage : backgroundImage];
   cell.backgroundView.autoresizingMask = UIViewAutoresizingFlexibleWidth | UIViewAutoresizingFlexibleHeight;
   cell.backgroundView.frame = cell.bounds;
  
   cell.textLabel.text = [tutorialNames objectAtIndex : indexPath.row];
   cell.imageView.image = [UIImage imageNamed : [tutorialIcons objectAtIndex : indexPath.row]];

   return cell;
}

//_________________________________________________________________
- (void) tableView : (UITableView *)tableView didSelectRowAtIndexPath : (NSIndexPath *)indexPath
{
   [self.detailViewController dismissPopover];
   [self.detailViewController setActiveDemo : demos[indexPath.row]];
}

//_________________________________________________________________
- (void) didReceiveMemoryWarning
{
   // Releases the view if it doesn't have a superview.
   [super didReceiveMemoryWarning];
   // Relinquish ownership any cached data, images, etc that aren't in use.
}

//_________________________________________________________________
- (void) viewDidUnload
{
   // Relinquish ownership of anything that can be recreated in viewDidLoad or on demand.
   // For example: self.myOutlet = nil;
}

//_________________________________________________________________
- (void) dealloc
{
   for (unsigned i = 0; i < nROOTDemos; ++i)
      delete demos[i];
}

//_________________________________________________________________
- (void) tableView : (UITableView *)tableView willDisplayCell : (UITableViewCell *)cell forRowAtIndexPath : (NSIndexPath *)indexPath
{
   [[cell textLabel] setBackgroundColor : [UIColor clearColor]];
   [[cell detailTextLabel] setBackgroundColor : [UIColor clearColor]];
}

@end
