# Skymatik Aero Website Style Guide

## Brand Identity
Based on the Digital Floats Controller manual cover, the Skymatik Aero brand represents precision, reliability, and advanced aviation technology. The style guide ensures consistency across all website elements.

## Color Palette

### Primary Colors
- **Deep Blue:** #0A3A5A (Darkest blue from gradient background)
- **Medium Blue:** #156091 (Middle tone from gradient background)
- **Light Blue:** #2A87C8 (Accent color for highlights and interactive elements)

### Secondary Colors
- **White:** #FFFFFF (Text on dark backgrounds, clean spaces)
- **Light Gray:** #F2F4F6 (Background for content sections)
- **Medium Gray:** #D1D5DB (Borders, dividers)
- **Dark Gray:** #4B5563 (Secondary text)

### Accent Colors
- **Alert Red:** #E53E3E (For safety bulletins and critical alerts)
- **Success Green:** #38A169 (For confirmation messages)

## Typography

### Headings
- **Font Family:** [Suggested: Roboto Condensed or similar bold sans-serif]
- **Main Headings (H1):**
  - All caps for main section titles (like "DIGITAL FLOATS CONTROLLER")
  - Font weight: 700 (Bold)
  - Letter spacing: 0.05em
  - Color: White on blue backgrounds, Deep Blue on light backgrounds

### Body Text
- **Font Family:** [Suggested: Open Sans or similar clean sans-serif]
- **Regular Text:**
  - Font weight: 400
  - Size: 16px base (responsive)
  - Line height: 1.6
  - Color: Dark Gray for main content, White for text on dark backgrounds

### Special Text Elements
- **Buttons:** Bold, uppercase text
- **Navigation:** Medium weight, uppercase for main navigation
- **Technical Specifications:** Monospace font for measurements and values

## Imagery

### Product Photography
- Professional, high-resolution images on subtle gradient backgrounds
- Blue color treatment consistent with manual cover
- Consistent lighting and angles across product line
- White/transparent background product cutouts for catalog views

### Aircraft Photography
- Professional aviation photography
- Blue overlay/filter treatment similar to manual cover
- Focus on aircraft that use Skymatik products
- Emphasis on seaplanes/floatplanes for Digital Floats Controller sections

### Icons
- Clean, minimal line icons
- Consistent weight and style
- Primary blue color or white (depending on background)
- Outlined style preferred over solid

## UI Elements

### Backgrounds
- Subtle grid pattern overlay on blue gradient backgrounds (matching manual cover)
- Light gray backgrounds for content-heavy sections
- White backgrounds for product details and technical information

### Buttons
- **Primary Buttons:**
  - Medium Blue background
  - White text
  - Slight rounded corners (4px radius)
  - Subtle blue glow on hover
- **Secondary Buttons:**
  - Transparent with blue border
  - Blue text
  - Same corner radius as primary
- **Tertiary/Text Buttons:**
  - No background or border
  - Blue text
  - Underline on hover

### Cards & Containers
- Subtle shadow (4px blur, 10% opacity)
- Light background
- 8px corner radius
- Thin border (1px) in Medium Gray
- Consistent padding (24px)

### Tables
- Clean, minimal design
- Alternating row colors for readability
- Column headers in Medium Blue with white text
- Borders only where needed for clarity

## Navigation

### Main Navigation
- Bold, uppercase text
- Clear hover states
- Active state indicator
- Dropdown menus for product categories

### Sub-Navigation
- Lighter weight than main navigation
- Clear hierarchy in dropdown menus
- Breadcrumb navigation on all pages below homepage

### Mobile Navigation
- Hamburger menu icon
- Slide-in menu panel
- Large touch targets (minimum 44px)

## Grid System
- 12-column responsive grid
- Consistent spacing using 8px increments
- Responsive breakpoints:
  - Mobile: 0-767px
  - Tablet: 768px-1023px
  - Desktop: 1024px+
- Container max-width: 1280px

## Interaction & Animation

### Hover States
- Subtle color shifts for buttons and links
- Scale transformations for cards (1.02x)
- Transition duration: 0.2s

### Focus States
- Clear keyboard focus indicators
- Blue outline (3px) for accessibility

### Page Transitions
- Subtle fade transitions between pages
- Quick loading states that maintain brand aesthetic

## Component-Specific Styles

### Product Listings
- Consistent image sizes
- Product name in bold
- Consistent information hierarchy
- Clear CTA buttons

### Bulletin Items
- Color-coding by bulletin type
- Consistent metadata display
- Priority indicators
- Clear status indicators (New, Updated, Resolved)

### Documentation Items
- Clear file type indicators
- Download size information
- Version and date information
- Preview capability where possible

## Accessibility Guidelines
- Minimum contrast ratio of 4.5:1 for normal text
- Minimum contrast ratio of 3:1 for large text
- Focus indicators visible at all times
- Non-text contrast of 3:1 for interactive elements
- Text resizing up to 200% without loss of content

## Implementation Notes
- Use CSS variables for colors and spacing
- Maintain consistent class naming convention
- Component-based architecture for reusability
- Mobile-first responsive approach
- Optimize images for web delivery 